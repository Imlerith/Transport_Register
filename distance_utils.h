/*
 * distance_utils.h
 *
 *  Created on: 27 Jul 2020
 *      Author: sergeynasekin
 */

#ifndef DISTANCE_UTILS_H_
#define DISTANCE_UTILS_H_

#pragma once

#include <cmath>

namespace Earth {
double ConvertDegreesToRadians(double degrees);

struct Point {
	double latitude;
	double longitude;

	static Point FromDegrees(double latitude, double longitude);
};

double Distance(Point lhs, Point rhs);
}

#endif /* DISTANCE_UTILS_H_ */
