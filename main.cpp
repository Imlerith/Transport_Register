/*
 * main.cpp
 *
 *  Created on: 26 Jul 2020
 *      Author: sergeynasekin
 */

#include "parser.h"
#include "json_lib.h"
#include "queries.h"
#include "distance_utils.h"
#include <iostream>
#include "general_utils.h"
#include "transport_register.h"

using namespace std;

int main() {
	const auto input_doc = Json::Load(cin);
	const auto& input_map = input_doc.GetRoot().AsMap();

	const TransportRegister db(
			BusOrStopInfo::ReadBusOrStopInfo(
					input_map.at("base_requests").AsArray()),
			input_map.at("routing_settings").AsMap());

	Json::PrintValue(
			Queries::ProcessAll(db, input_map.at("stat_requests").AsArray()),
			cout);
	cout << endl;

	return 0;
}

