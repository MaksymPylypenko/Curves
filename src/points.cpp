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

Points Points::catmullRomLerp(int i0, int i1, int i2, int i3) {

	glm::vec2 a = getPosition(i0);
	glm::vec2 b = getPosition(i1);
	glm::vec2 c = getPosition(i2);
	glm::vec2 d = getPosition(i3);

	Points curve;
	for (float t = 1; t > 0; t -= 0.02f)
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

//Points RecursiveLerp(Points cp) {
//	Points curve;
//	for (int i = 1; i < cp.numElements()-1; i++) {
//		Points subCurve = Lerp(cp.getPosition(i - 1), cp.getPosition(i), cp.getPosition(i + 1), cp.getPosition(i + 2));
//		curve.extend(subCurve);
//	}	
//	return curve;
//}


int Points::lineLerp(int positionsSize, int colorsSize, float t)
{
	int diff = colorsSize - positionsSize;

	return positionsSize + (diff * t);
}

// Derrived from https://stackoverflow.com/questions/37642168/how-to-convert-quadratic-bezier-curve-code-into-cubic-bezier-curve/37642695#37642695
Points Points::BezierLerp(int i0, int i1, int i2, int i3) {

	glm::vec2 a = getPosition(i0);
	glm::vec2 b = getPosition(i1);
	glm::vec2 c = getPosition(i2);
	glm::vec2 d = getPosition(i3);

	Points curve;
	for (float i = 0; i < 1; i += 0.01)
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