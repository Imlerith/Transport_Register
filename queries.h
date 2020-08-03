/*
 * queries.h
 *
 *  Created on: 27 Jul 2020
 *      Author: sergeynasekin
 */

#ifndef QUERIES_H_
#define QUERIES_H_

#pragma once

#include "json_lib.h"
#include "transport_register.h"

#include <string>
#include <variant>

namespace Queries {
struct Stop {
	std::string name;

	Json::Dict Process(const TransportRegister& db) const;
};

struct Bus {
	std::string name;

	Json::Dict Process(const TransportRegister& db) const;
};

struct Route {
	std::string stop_from;
	std::string stop_to;

	Json::Dict Process(const TransportRegister& db) const;
};

std::variant<Stop, Bus, Route> Read(const Json::Dict& attrs);

std::vector<Json::Node> ProcessAll(const TransportRegister& db,
		const std::vector<Json::Node>& requests);
}

#endif /* QUERIES_H_ */
