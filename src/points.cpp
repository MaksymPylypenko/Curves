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

void Points::erase(int n) {
	for (int i = 0; i <= n; i++) {
		points.pop_back();
	}
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

	int i = points.size() - 3;

	if (type == CatmullRom) {
		if (i >= 0) { // add additional last point
			Points subCurve = lerp(i, i + 1, i + 2, i + 2, type);
			finalCurve.extend(subCurve);
		}
	}
	

	while (i >= 1) {
		Points subCurve = lerp(i - 1, i, i + 1, i + 2, type);
		finalCurve.extend(subCurve);
		i--;
	}	

	if (type == CatmullRom) {
		if (i == 0) { // add additional 1st point
			Points subCurve = lerp(i, i, i + 1, i + 2, type);
			finalCurve.extend(subCurve);
		}
	}
	return finalCurve;
}


Points Points::lerp(int i0, int i1, int i2, int i3, int type) {		
	if (type == CatmullRom) {
		return catmullRomLerp(i0, i1, i2, i3);
	}
	else if (type == Bezier) {
		return bezierLerp(i0, i1, i2, i3);
	}
	else {
		return bSplineLerp(i0, i1, i2, i3);
	}
}

Points Points::lerp(Point newPoint, int type) {
	add(newPoint);

	Points finalCurve;
		
	int i = points.size()-3; // interpolation starts at this point

	if (type == CatmullRom) {
		if (i == 0) { // allow to lerp from the first point		 
			finalCurve.extend(lerp(i, i + 1, i + 2, i + 2, type));
			finalCurve.extend(lerp(i, i, i + 1, i + 2, type));
			return finalCurve;
		}
	}

	if (i > 0) {
		if (type == CatmullRom) {
			finalCurve.extend(lerp(i, i + 1, i + 2, i + 2, type));
		}
		finalCurve.extend(lerp(i - 1, i, i + 1, i + 2, type));			
		return finalCurve;
	}

	else {
		return Points();
	}

}

// -------------------------------------------
// Weighted combination of the Bernstein basis polynomials
// http://www.joshbarczak.com/blog/?p=730


Points Points::bezierLerp(int i0, int i1, int i2, int i3) {

	Points curve;
	if (i0 % 3 == 0) {

		glm::vec2 a = getPosition(i0);
		glm::vec2 b = getPosition(i1);
		glm::vec2 c = getPosition(i2);
		glm::vec2 d = getPosition(i3);

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


// Alternative representations

//Points Points::bezierLerp(int i0, int i1, int i2, int i3) {
//
//	glm::vec2 a = getPosition(i0);
//	glm::vec2 b = getPosition(i1);
//	glm::vec2 c = getPosition(i2);
//	glm::vec2 d = getPosition(i3);
//
//	glm::mat4 m = glm::mat4(-1, 3, -3, 1, 3, -6, 3, 0, -3, 3, 0, 0, 1, 0, 0, 0);
//
//	glm::vec4 cp_x = glm::vec4(a.x, b.x, c.x, d.x);
//	glm::vec4 cp_y = glm::vec4(a.y, b.y, c.y, d.y);
//
//
//	Points curve;
//
//	// Fast
//	for (float t = 1; t > 0; t -= T)
//	{
//		float tSq = t * t;
//		float tQu = tSq * t;
//			
//		glm::vec4 tv = glm::vec4(tQu, tSq, t, 1);	
//		glm::vec4 product = m * tv;
//
//		float currX = glm::dot(product,cp_x);
//		float currY = glm::dot(product, cp_y);
//
//		curve.add(Point(glm::vec2(currX, currY), glm::vec4(0.9, 0.3, 0.3, 1)));
//	}
//	return curve;
//}


//Points Points::catmullRomLerp(int i0, int i1, int i2, int i3) {
//
//	glm::vec2 a = getPosition(i0);
//	glm::vec2 b = getPosition(i1);
//	glm::vec2 c = getPosition(i2);
//	glm::vec2 d = getPosition(i3);
//
//	glm::mat4 m = glm::mat4(2, -2, 1, 1, -3, 3, -2, -1, 0, 0, 1, 0, 1, 0, 0, 0);
//	glm::mat4 k = glm::mat4(0, 1, 0, 0, 0, 0, 1, 0, -0.5, 0, 0.5, 0, 0, -0.5, 0, 0.5);
//	m = k * m;
//
//	glm::vec4 cp_x = glm::vec4(a.x, b.x, c.x, d.x);
//	glm::vec4 cp_y = glm::vec4(a.y, b.y, c.y, d.y);
//
//
//	Points curve;
//
//	// Fast
//	for (float t = 1; t > 0; t -= T)
//	{
//		float tSq = t * t;
//		float tQu = tSq * t;
//
//		glm::vec4 tv = glm::vec4(tQu, tSq, t, 1);
//		glm::vec4 product = m * tv;
//
//		float currX = glm::dot(product, cp_x);
//		float currY = glm::dot(product, cp_y);
//
//		curve.add(Point(glm::vec2(currX, currY), glm::vec4(0.9, 0.3, 0.3, 1)));
//	}
//	return curve;
//}