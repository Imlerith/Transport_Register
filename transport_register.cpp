/*
 * transport_catalog.cpp
 *
 *  Created on: 27 Jul 2020
 *      Author: sergeynasekin
 */

#include "transport_register.h"

#include <sstream>

using namespace std;

TransportRegister::TransportRegister(vector<BusOrStopInfo::InputQuery> data,
		const Json::Dict& routing_settings_json) {

	auto stops_end = partition(begin(data), end(data), [](const auto& item) {
		return holds_alternative<BusOrStopInfo::Stop>(item);
	});

	// filter stops
	BusOrStopInfo::StopsDict stops_dict;
	for (const auto& item : Range { begin(data), stops_end }) {
		const auto& stop = get<BusOrStopInfo::Stop>(item);
		stops_dict[stop.name] = &stop;
		stops_.insert( { stop.name, { } });
	}

	// filter buses
	BusOrStopInfo::BusesDict buses_dict;
	for (const auto& item : Range { stops_end, end(data) }) {
		const auto& bus = get<BusOrStopInfo::Bus>(item);

		buses_dict[bus.name] = &bus;
		buses_[bus.name] = Bus { bus.stops.size(), ComputeUniqueItemsCount(
				AsRange(bus.stops)), ComputeRoadRouteLength(bus.stops,
				stops_dict), ComputeGeoRouteDistance(bus.stops, stops_dict) };

		for (const string& stop_name : bus.stops) {
			stops_.at(stop_name).bus_names.insert(bus.name);
		}
	}

	router_ = make_unique<TransportRouter>(stops_dict, buses_dict,
			routing_settings_json);
}

const TransportRegister::Stop* TransportRegister::GetStop(
		const string& name) const {
	return GetValuePointer(stops_, name);
}

const TransportRegister::Bus* TransportRegister::GetBus(
		const string& name) const {
	return GetValuePointer(buses_, name);
}

optional<TransportRouter::RouteInfo> TransportRegister::FindRoute(
		const string& stop_from, const string& stop_to) const {
	// delegate route finding to a function from router
	return router_->FindRoute(stop_from, stop_to);
}

int TransportRegister::ComputeRoadRouteLength(const vector<string>& stops,
		const BusOrStopInfo::StopsDict& stops_dict) {
	int result = 0;
	for (size_t i = 1; i < stops.size(); ++i) {
		result += BusOrStopInfo::ComputeStopsDistance(
				*stops_dict.at(stops[i - 1]), *stops_dict.at(stops[i]));
	}
	return result;
}

double TransportRegister::ComputeGeoRouteDistance(const vector<string>& stops,
		const BusOrStopInfo::StopsDict& stops_dict) {
	double result = 0;
	for (size_t i = 1; i < stops.size(); ++i) {
		result += Earth::Distance(stops_dict.at(stops[i - 1])->position,
				stops_dict.at(stops[i])->position);
	}
	return result;
}

