#ifndef POINTS_H // include guard
#define POINTS_H

#include "common.h"
#include "geometry.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

class Point {
public:
	glm::vec2 position;
	glm::vec4 color;
	Point(glm::vec2 position, glm::vec4 color);
};

// Holds a sequence of points (e.g control points or points that form a curve)
class Points {
public:
	std::vector<Point> points;
	void add(Point p);
	Point pop();
	glm::vec2 getPosition(int i);
	int numElements();
	void extend(Points b);
	Geometry extractGeometry();
	Points catmullRomLerp(int i0, int i1, int i2, int i3);
	int lineLerp(int positionsSize, int colorsSize, float t);
	Points BezierLerp(int i0, int i1, int i2, int i3);
};


#endif POINTS_H