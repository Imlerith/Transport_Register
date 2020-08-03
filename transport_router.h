/*
 * transport_router.h
 *
 *  Created on: 27 Jul 2020
 *      Author: sergeynasekin
 */

#ifndef TRANSPORT_ROUTER_H_
#define TRANSPORT_ROUTER_H_

#pragma once

#include "parser.h"
#include "graph.h"
#include "json_lib.h"
#include "router.h"

#include <memory>
#include <unordered_map>
#include <vector>

class TransportRouter {
private:
	using BusGraph = Graph::DirectedWeightedGraph<double>;
	using Router = Graph::Router<double>;

public:
	TransportRouter(const BusOrStopInfo::StopsDict& stops_dict,
			const BusOrStopInfo::BusesDict& buses_dict,
			const Json::Dict& routing_settings_json);

	struct RouteInfo {
		double total_time;

		struct BusItem {
			std::string bus_name;
			double time;
			size_t span_count;
		};

		struct WaitItem {
			std::string stop_name;
			double time;
		};

		using Item = std::variant<BusItem, WaitItem>;
		std::vector<Item> items;
	};

	std::optional<RouteInfo> FindRoute(const std::string& stop_from,
			const std::string& stop_to) const;

private:
	struct RoutingSettings {
		int bus_wait_time;  // in minutes
		double bus_speed;  // km/h
	};

	static RoutingSettings MakeRoutingSettings(const Json::Dict& json);

	void FillGraphWithStops(const BusOrStopInfo::StopsDict& stops_dict);

	void FillGraphWithBuses(const BusOrStopInfo::StopsDict& stops_dict,
			const BusOrStopInfo::BusesDict& buses_dict);

	struct StopVertexIds {
		Graph::VertexId in;
		Graph::VertexId out;
	};

	struct VertexInfo {
		std::string stop_name;
	};

	struct BusEdgeInfo {
		std::string bus_name;
		size_t span_count;
	};

	struct WaitEdgeInfo {
	};

	using EdgeInfo = std::variant<BusEdgeInfo, WaitEdgeInfo>;

	RoutingSettings routing_settings_;
	BusGraph graph_;
	std::unique_ptr<Router> router_;
	std::unordered_map<std::string, StopVertexIds> stops_vertex_ids_;  // map from stop name to its corresponding in- and out-vertices
	std::vector<VertexInfo> vertices_info_;
	std::vector<EdgeInfo> edges_info_;
};

#endif /* TRANSPORT_ROUTER_H_ */
