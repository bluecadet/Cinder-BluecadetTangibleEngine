#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "ideum/tangibleengine/tangible_tracker.h"

namespace bluecadet {
namespace tangibleengine {

class TangibleUtils {

public:

static ci::vec2 getPatternCenter(const TE_Pattern & pattern, bool useAverage = false) {

	if (!useAverage && pattern.num_points >= 3) {
		// See "Equation of a Circle from 3 Points (2 dimensions)" on http://paulbourke.net/geometry/circlesphere/

		// choose any three points
		TE_Point p1 = pattern.points[0];
		TE_Point p2 = pattern.points[1];
		TE_Point p3 = pattern.points[2];

		// try to avoid vertical perpendiculars/division by zero by swapping points
		if (p1.x == -p2.x) {
			auto temp = p2;
			p2 = p3;
			p3 = temp;
		}
		if (p3.x == -p2.x) {
			auto temp = p2;
			p2 = p1;
			p1 = temp;
		}

		if (p1.x == -p2.x || p2.x == -p3.x) {
			// can't solve w/o division of zero, so return avg instead
			return getPatternCenter(pattern, true);
		}

		// calc slopes of lines between p1,p2 and p2,p3
		const float mA = (p2.y - p1.y) / (p2.x - p1.x);
		const float mB = (p3.y - p2.y) / (p3.x - p2.x);

		if (mA == -mB) {
			// can only happen if lines are parallel, which means that this is not a circle
			return getPatternCenter(pattern, true);
		}

		ci::vec2 center;

		center.x = (mA * mB * (p1.y - p3.y) + mB * (p1.x + p2.x) - mA * (p2.x + p3.x)) / (2.0f * (mB - mA));
		center.y = - (1.0f / mA) * (center.x - (p1.x + p2.x) / 2.0f) + (p1.y + p2.y) / 2.0f;

		return center;

	} else {
		// Find avg; this can lead to skewed results and is
		// not very accurate for asymmetrical patterns
		ci::vec2 center;
		for (int i = 0; i < pattern.num_points; ++i) {
			center += ci::vec2(pattern.points[i].x, pattern.points[i].y);
		}
		center /= (float)pattern.num_points;
		return center;
	}
}

static float getPatternRadius(const TE_Pattern & pattern) {
	const ci::vec2 center = getPatternCenter(pattern);
	float radius = 0;
	for (int i = 0; i < pattern.num_points; ++i) {
		const ci::vec2 point = ci::vec2(pattern.points[i].x, pattern.points[i].y) - center;
		radius = glm::max(glm::length(point), radius);
	}
	return radius;
}

static std::vector<ci::vec2> getLocalizedPatternPoints(const TE_Pattern & pattern) {
	const ci::vec2 center = getPatternCenter(pattern);
	std::vector<ci::vec2> points(pattern.num_points);
	for (int i = 0; i < pattern.num_points; ++i) {
		const ci::vec2 point = ci::vec2(pattern.points[i].x, pattern.points[i].y) - center;
		points[i] = point;
	}
	return points;
}

private:
	// Don't construct this class; it's just there for the namespace
	TangibleUtils() {};
	~TangibleUtils() {};

};

}
}
