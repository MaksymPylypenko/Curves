#include "points.h"

Point::Point(glm::vec2 position, glm::vec4 color)
{
	this->position = position;
	this->color = color;
}


void Points::add(Point p)
{
	points.push_back(p);
}


Point Points::pop() {
	Point ret = points.back();
	points.pop_back();
	return ret;
}


glm::vec2 Points::getPosition(int i) {
	return points[i].position;
}


int Points::numElements() {
	return points.size();
}


void Points::extend(Points b) {
	points.insert(points.end(), b.points.begin(), b.points.end());
}

Geometry Points::extractGeometry() {
	std::vector<GLfloat> positions;
	std::vector<GLfloat> colors;
	std::vector<GLint> indices;

	for (std::size_t i = 0; i < points.size(); i++)
	{
		positions.push_back(points[i].position.x);
		positions.push_back(points[i].position.y);
		colors.push_back(points[i].color.r);
		colors.push_back(points[i].color.g);
		colors.push_back(points[i].color.b);
		colors.push_back(points[i].color.a);
		indices.push_back(indices.size());
	}
	return Geometry(positions, colors, indices);
}


Points Points:: lerp(int type) {
	Points finalCurve;
	for (int i = points.size()-3; i > 0; i--) {
		Points subCurve = lerp(i - 1, i, i + 1, i + 2, type);
		finalCurve.extend(subCurve);
	}	
	return finalCurve;
}


Points Points::lerp(int i0, int i1, int i2, int i3, int type) {		
	if (type == 0) {
		return catmullRomLerp(i0, i1, i2, i3);
	}
	else if (type == 1) {
		return bezierLerp(i0, i1, i2, i3);
	}
	else {
		return bSplineLerp(i0, i1, i2, i3);
	}
}

Points Points::lerp(Point newPoint, int type) {
	add(newPoint);
	int i = points.size()-3; // interpolation starts at this point
	if (i > 0) {
		return lerp(i - 1, i, i + 1, i + 2, type);	
	}
	else {
		return Points();
	}

}

// -------------------------------------------
// Weighted combination of the Bernstein basis polynomials
// http://www.joshbarczak.com/blog/?p=730


Points Points::bezierLerp(int i0, int i1, int i2, int i3) {

	glm::vec2 a = getPosition(i0);
	glm::vec2 b = getPosition(i1);
	glm::vec2 c = getPosition(i2);
	glm::vec2 d = getPosition(i3);

	Points curve;

	// Fast
	for (float t = 1; t > 0; t -= T)
	{
		float tSq = t * t;
		float tQu = tSq * t;
		float oneMinusT = 1.0f - t;
		float oneMinusTSq = oneMinusT * oneMinusT;
	
		float q1 = oneMinusTSq * oneMinusT;
		float q2 = 3.0f * t * oneMinusTSq;
		float q3 = 3.0f * tSq * oneMinusT;
		float q4 = tQu;

		float currX = a.x * q1 + b.x * q2 + c.x * q3 + d.x * q4;
		float currY = a.y * q1 + b.y * q2 + c.y * q3 + d.y * q4;

		curve.add(Point(glm::vec2(currX, currY), glm::vec4(0.9, 0.3, 0.3, 1)));
	}
	return curve;
}


Points Points::catmullRomLerp(int i0, int i1, int i2, int i3) {

	glm::vec2 a = getPosition(i0);
	glm::vec2 b = getPosition(i1);
	glm::vec2 c = getPosition(i2);
	glm::vec2 d = getPosition(i3);

	Points curve;
	for (float t = 1; t > 0; t -= T)
	{
		float tSq = t * t;
		float tQu = tSq * t;

		float q1 = -tQu + 2.0f * tSq - t;
		float q2 = 3.0f * tQu - 5.0f * tSq + 2.0f;
		float q3 = -3.0f * tQu + 4.0f * tSq + t;
		float q4 = tQu - tSq;

		float currX = 0.5f * (a.x * q1 + b.x * q2 + c.x * q3 + d.x * q4);
		float currY = 0.5f * (a.y * q1 + b.y * q2 + c.y * q3 + d.y * q4);

		curve.add(Point(glm::vec2(currX, currY), glm::vec4(0.9, 0.3, 0.3, 1)));
	}
	return curve;
}

Points Points::bSplineLerp(int i0, int i1, int i2, int i3) {
	glm::vec2 a = getPosition(i0);
	glm::vec2 b = getPosition(i1);
	glm::vec2 c = getPosition(i2);
	glm::vec2 d = getPosition(i3);

	Points curve;
	for (float t = 1; t > 0; t -= T)
	{
		float tSq = t * t;
		float tQu = tSq * t;
				
		float q1 = -tQu + 3.0f * tSq - 3.0f * t + 1;
		float q2 = 3.0f * tQu - 6.0f * tSq + 4;
		float q3 = -3 * tQu + 3.0f * tSq + 3.0f * t + 1;
		float q4 = tQu;

		float currX = (a.x * q1 + b.x * q2 + c.x * q3 + d.x * q4) / 6.0f;
		float currY = (a.y * q1 + b.y * q2 + c.y * q3 + d.y * q4) / 6.0f;

		curve.add(Point(glm::vec2(currX, currY), glm::vec4(0.9, 0.3, 0.3, 1)));
	}
	return curve;
}
