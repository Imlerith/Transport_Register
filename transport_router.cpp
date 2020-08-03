/*
 * transport_router.cpp
 *
 *  Created on: 27 Jul 2020
 *      Author: sergeynasekin
 */

#include "transport_router.h"

using namespace std;

TransportRouter::TransportRouter(const BusOrStopInfo::StopsDict& stops_dict,
		const BusOrStopInfo::BusesDict& buses_dict,
		const Json::Dict& routing_settings_json) :
		routing_settings_(MakeRoutingSettings(routing_settings_json)) {

	// initialize the underlying graph with the count of vertices
	const size_t vertex_count = stops_dict.size() * 2;
	vertices_info_.resize(vertex_count);
	graph_ = BusGraph(vertex_count);

	FillGraphWithStops(stops_dict);
	FillGraphWithBuses(stops_dict, buses_dict);

	// the router, when constructed, finds optimal routes for every vertex
	// it can do that at this moment because all buses and stops have been added to the graph
	router_ = std::make_unique<Router>(graph_);
}

TransportRouter::RoutingSettings TransportRouter::MakeRoutingSettings(
		const Json::Dict& json) {
	return {
		json.at("bus_wait_time").AsInt(),
		json.at("bus_speed").AsDouble(),
	};
}

void TransportRouter::FillGraphWithStops(
		const BusOrStopInfo::StopsDict& stops_dict) {
	Graph::VertexId vertex_id = 0;

	for (const auto& stops_pair : stops_dict) {
		const auto& stop_name = stops_pair.first;
		auto& vertex_ids = stops_vertex_ids_[stop_name];
		// each stop corresponds to two vertices
		// for each stop (vertex) generate the ids of two vertices corresponding to it
		vertex_ids.in = vertex_id++;
		vertex_ids.out = vertex_id++;
		vertices_info_[vertex_ids.in] = {stop_name};
		vertices_info_[vertex_ids.out] = {stop_name};

		edges_info_.push_back(WaitEdgeInfo { });

		// add the edge between the stop vertices with the weight equal to bus wait time
		const Graph::EdgeId edge_id = graph_.AddEdge(
				{ vertex_ids.out, vertex_ids.in,
						static_cast<double>(routing_settings_.bus_wait_time) });
		assert(edge_id == edges_info_.size() - 1);
	}

	assert(vertex_id == graph_.GetVertexCount());
}

void TransportRouter::FillGraphWithBuses(
		const BusOrStopInfo::StopsDict& stops_dict,
		const BusOrStopInfo::BusesDict& buses_dict) {

	for (const auto& buses_pair : buses_dict) {

		const auto& bus_item = buses_pair.second;  // pointer to bus
		const auto& bus = *bus_item;  // get the bus behind the pointer
		const size_t stop_count = bus.stops.size(); // how many stops the bus goes through

		if (stop_count <= 1) {
			continue;
		}
		// function to calculate distances between two consecutive stops in a bus route
		auto compute_distance_from =
				[&stops_dict, &bus](size_t lhs_idx) {
					return BusOrStopInfo::ComputeStopsDistance(*stops_dict.at(bus.stops[lhs_idx]),
							*stops_dict.at(bus.stops[lhs_idx + 1]));
				};
		// get the total distance for a bus
		for (size_t start_stop_idx = 0; start_stop_idx + 1 < stop_count;
				++start_stop_idx) {
			const Graph::VertexId start_vertex =
					stops_vertex_ids_[bus.stops[start_stop_idx]].in;  // start at the first in-vertex
			int total_distance = 0;
			// trace each possible route starting at a given stop and incrementally add edges on the way
			// as well as edges' counts for sub-routes
			for (size_t finish_stop_idx = start_stop_idx + 1;
					finish_stop_idx < stop_count; ++finish_stop_idx) {
				total_distance += compute_distance_from(finish_stop_idx - 1);
				edges_info_.push_back(BusEdgeInfo { .bus_name = bus.name,
						.span_count = finish_stop_idx - start_stop_idx, });
				const Graph::EdgeId edge_id =
						graph_.AddEdge(
								{ start_vertex,
										stops_vertex_ids_[bus.stops[finish_stop_idx]].out,
										total_distance * 1.0
												/ (routing_settings_.bus_speed
														* 1000.0 / 60) // m / (km/h * 1000 / 60) = min
								});
				assert(edge_id == edges_info_.size() - 1);
			}
		}
	}
}

optional<TransportRouter::RouteInfo> TransportRouter::FindRoute(
		const string& stop_from, const string& stop_to) const {
	const Graph::VertexId vertex_from = stops_vertex_ids_.at(stop_from).out;
	const Graph::VertexId vertex_to = stops_vertex_ids_.at(stop_to).out;
	// when this method is called, all optimal routes have already been calculated
	const auto route = router_->BuildRoute(vertex_from, vertex_to);
	if (!route) {
		return nullopt;
	}
	// now it only remains to "backtrack" the optimal route and collect route info
	// (on travel time, wait time, stops etc.)
	RouteInfo route_info = { .total_time = route->weight };
	route_info.items.reserve(route->edge_count);
	for (size_t edge_idx = 0; edge_idx < route->edge_count; ++edge_idx) {
		const Graph::EdgeId edge_id = router_->GetRouteEdge(route->id,
				edge_idx);
		const auto& edge = graph_.GetEdge(edge_id);
		const auto& edge_info = edges_info_[edge_id];
		if (holds_alternative<BusEdgeInfo>(edge_info)) {
			const BusEdgeInfo& bus_edge_info = get<BusEdgeInfo>(edge_info);
			route_info.items.push_back(RouteInfo::BusItem { .bus_name =
					bus_edge_info.bus_name, .time = edge.weight, .span_count =
					bus_edge_info.span_count, });
		} else {
			const Graph::VertexId vertex_id = edge.from;
			route_info.items.push_back(
					RouteInfo::WaitItem { .stop_name =
							vertices_info_[vertex_id].stop_name, .time =
							edge.weight, });
		}
	}

	router_->RemoveRoute(route->id);
	return route_info;
}

