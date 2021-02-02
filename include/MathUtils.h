#ifndef __MATH_UTILS_H
#define __MATH_UTILS_H

#include "olcPixelGameEngine.h"

struct AABB {
	olc::vd2d pos;
	olc::vd2d size;
	inline AABB(double x, double y, double w, double h) : pos{x, y}, size{w, h} {}
	inline AABB() {}

	inline bool isInside(olc::vd2d point) {
		if (point.x > pos.x &&
			point.x < pos.x + size.x &&
			point.y > pos.y &&
			point.y < pos.y + size.y) {
			return true;
		} else {
			return false;
		}
	}
};

#endif