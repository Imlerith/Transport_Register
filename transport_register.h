/*
 * transport_catalog.h
 *
 *  Created on: 27 Jul 2020
 *      Author: sergeynasekin
 */

#ifndef TRANSPORT_REGISTER_H_
#define TRANSPORT_REGISTER_H_

#pragma once

#pragma once

#include "parser.h"
#include "json_lib.h"
#include "transport_router.h"
#include "general_utils.h"

#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace Responses {
struct Stop {
	std::set<std::string> bus_names;
};

struct Bus {
	size_t stop_count = 0;
	size_t unique_stop_count = 0;
	int road_route_length = 0;  // shortest road route
	double geo_route_length = 0.0;  // shortest geo route
};
}

class TransportRegister {
private:
	using Bus = Responses::Bus;
	using Stop = Responses::Stop;

public:
	// there are two different structures for Bus: one in the namespace
	// BusOrStopInfo, the other in the namespace Responses
	TransportRegister(std::vector<BusOrStopInfo::InputQuery> data,
			const Json::Dict& routing_settings_json);

	const Stop* GetStop(const std::string& name) const;
	const Bus* GetBus(const std::string& name) const;

	std::optional<TransportRouter::RouteInfo> FindRoute(
			const std::string& stop_from, const std::string& stop_to) const;

	std::string RenderMap() const;

private:
	static int ComputeRoadRouteLength(const std::vector<std::string>& stops,
			const BusOrStopInfo::StopsDict& stops_dict);

	static double ComputeGeoRouteDistance(const std::vector<std::string>& stops,
			const BusOrStopInfo::StopsDict& stops_dict);

	std::unordered_map<std::string, Stop> stops_;
	std::unordered_map<std::string, Bus> buses_;
	std::unique_ptr<TransportRouter> router_;
};

#endif /* TRANSPORT_REGISTER_H_ */
