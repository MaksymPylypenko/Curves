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
// Custom lerps
// http://www.joshbarczak.com/blog/?p=730

float lineLerp(float a, float b, float t)
{
	return b + ((a-b) * t);
}

// Derrived from https://stackoverflow.com/questions/37642168/how-to-convert-quadratic-bezier-curve-code-into-cubic-bezier-curve/37642695#37642695
Points Points::bezierLerp(int i0, int i1, int i2, int i3) {

	glm::vec2 a = getPosition(i0);
	glm::vec2 b = getPosition(i1);
	glm::vec2 c = getPosition(i2);
	glm::vec2 d = getPosition(i3);

	Points curve;
	for (float i = 0; i < 1; i += T)
	{
		// The Green Lines
		float xa = lineLerp(a.x, b.x, i);
		float ya = lineLerp(a.y, b.y, i);
		float xb = lineLerp(b.x, c.x, i);
		float yb = lineLerp(b.y, c.y, i);
		float xc = lineLerp(c.x, d.x, i);
		float yc = lineLerp(c.y, d.y, i);

		// The Blue Line
		float xm = lineLerp(xa, xb, i);
		float ym = lineLerp(ya, yb, i);
		float xn = lineLerp(xb, xc, i);
		float yn = lineLerp(yb, yc, i);

		// The Black Dot
		float currX = lineLerp(xm, xn, i);
		float currY = lineLerp(ym, yn, i);
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

// Article https://tiborstanko.sk/teaching/geo-num-2016/tp3.html
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
