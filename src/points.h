#ifndef POINTS_H // include guard
#define POINTS_H

#include "common.h"
#include "geometry.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

const float T = 0.005; // increment step for linear interpolation

class Point {
public:
	glm::vec2 position;
	glm::vec4 color;
	Point(glm::vec2 position, glm::vec4 color);
};


// Holds a sequence of points (e.g control points or points that form a curve)
class Points {
private:
	Points catmullRomLerp(int i0, int i1, int i2, int i3);
	Points bezierLerp(int i0, int i1, int i2, int i3);
	Points bSplineLerp(int i0, int i1, int i2, int i3);
public:
	std::vector<Point> points;

	void add(Point p);
	Point pop();
	glm::vec2 getPosition(int i);
	int numElements();
	void extend(Points b);
	Geometry extractGeometry();	
	Points lerp(int type);
	Points lerp(int i0, int i1, int i2, int i3, int type);
	Points lerp(Point, int type);
};

#endif POINTS_H