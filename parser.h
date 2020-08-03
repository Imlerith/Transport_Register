/*
 * parser.h
 *
 *  Created on: 27 Jul 2020
 *      Author: sergeynasekin
 */

#ifndef PARSER_H_
#define PARSER_H_

#pragma once

#include "json_lib.h"
#include "distance_utils.h"

#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace BusOrStopInfo {
struct Stop {
	std::string name;
	Earth::Point position;
	std::unordered_map<std::string, int> distances;

	static Stop ParseFrom(const Json::Dict& attrs);
};

int ComputeStopsDistance(const Stop& lhs, const Stop& rhs);

std::vector<std::string> ParseStops(const std::vector<Json::Node>& stop_nodes,
		bool is_roundtrip);

struct Bus {
	std::string name;
	std::vector<std::string> stops;

	static Bus ParseFrom(const Json::Dict& attrs);
};

using InputQuery = std::variant<Stop, Bus>;

std::vector<InputQuery> ReadBusOrStopInfo(const std::vector<Json::Node>& nodes);

template<typename Object>
using Dict = std::unordered_map<std::string, const Object*>;

using StopsDict = Dict<Stop>;
using BusesDict = Dict<Bus>;
}

#endif /* PARSER_H_ */
