#ifndef GEOMETRY_H // include guard
#define GEOMETRY_H

#include "common.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

class Geometry {
public:
	std::vector<GLfloat> positions;
	std::vector<GLfloat> colors;
	std::vector<GLint> indices;

	Geometry();
	Geometry(std::vector<GLfloat> positions, std::vector<GLfloat> colors, std::vector<GLint> indices);

	void add(glm::vec2 position, glm::vec4 color);
	int numElements();

	void load(int vao, GLuint vPosition, GLuint vColor);
	Geometry subData(float t);	
};

#endif GEOMETRY_H