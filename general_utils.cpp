/*
 * utils.cpp
 *
 *  Created on: 26 Jul 2020
 *      Author: sergeynasekin
 */

#include "general_utils.h"

#include <cctype>

using namespace std;

string_view Strip(string_view line) {
	while (!line.empty() && isspace(line.front())) {
		line.remove_prefix(1);
	}
	while (!line.empty() && isspace(line.back())) {
		line.remove_suffix(1);
	}
	return line;
}

