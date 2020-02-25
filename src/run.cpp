#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "common.h"

#include <iostream>
#include <vector>

const char* WINDOW_TITLE = "curves";
const double FRAME_RATE_MS = 1000.0 / 60.0;

//----------------------------------------------------------------------------

// Holds data that will be moved to GPU
class Geometry {
public:
	std::vector<GLfloat> positions;
	std::vector<GLfloat> colors;
	std::vector<GLint> indices;

	//GLuint vao;
	//GLuint vPosition;
	//GLuint vColor;

	Geometry() {
		positions = std::vector<GLfloat>();
		colors = std::vector<GLfloat>();
		indices = std::vector<GLint>();
	}

	Geometry(std::vector<GLfloat> positions, std::vector<GLfloat> colors, std::vector<GLint> indices)
	{
		this->positions = positions;
		this->colors = colors;
		this->indices = indices;
	}

	void add(glm::vec2 position, glm::vec4 color) {
		positions.push_back(position.x);
		positions.push_back(position.y);
		colors.push_back(color.r);
		colors.push_back(color.g);
		colors.push_back(color.b);
		colors.push_back(color.a);
		indices.push_back(indices.size());
	}

	int numElements() {
		return indices.size();
	}

	//void load()
	//{
	//	load(vao, vPosition, vColor);
	//}

	void load(GLuint vao, GLuint vPosition, GLuint vColor) {
		//this->vao = vao;
		//this->vPosition = vPosition;
		//this->vColor = vColor;

		std::size_t N1 = sizeof(GLfloat) * numElements() * 2;
		std::size_t N2 = sizeof(GLfloat) * numElements() * 4;
		std::size_t N3 = N1 + N2;
		std::size_t K1 = sizeof(GLuint) * numElements();

		GLuint buffer;
		glBindVertexArray(vao);

		// Create and initialize a buffer object
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, N3, NULL, GL_DYNAMIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, N1, positions.data());
		glBufferSubData(GL_ARRAY_BUFFER, N1, N2, colors.data());

		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, K1, indices.data(), GL_DYNAMIC_DRAW);

		// Set up arrays	
		glEnableVertexAttribArray(vPosition);
		glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

		glEnableVertexAttribArray(vColor);
		glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(N1));						
	}
};

//----------------------------------------------------------------------------

class Point {
public:
	glm::vec2 position;
	glm::vec4 color;
	Point(glm::vec2 position, glm::vec4 color) {
		this->position = position;
		this->color = color;
	}
};

//----------------------------------------------------------------------------

// Holds a sequence of points (e.g cp points or points that form a curve)
class Points {
public:
	std::vector<Point> points;

	void add(Point p) {
		points.push_back(p);
	}

	void remove() {
		points.pop_back();
	}

	Point pop() {
		Point ret = points.back();
		points.pop_back();
		return ret;
	}

	glm::vec2 getPosition(int i) {
		return points[i].position;
	}

	int numElements() {
		return points.size();
	}

	void extend(Points b) {
		points.insert(points.end(), b.points.begin(), b.points.end());
	}

	Geometry extractGeometry() {
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

	Points catmullRomLerp(int i0, int i1, int i2, int i3) {
		
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

	Points lerp() {
		Points finalCurve;
		Points subCurve;

		for(int i = numElements() - 3; i >= 1; i--)
		{
			subCurve = catmullRomLerp(i - 1, i, i + 1, i + 2);
			finalCurve.extend(subCurve);
		}
		return finalCurve;
	}

	int lineLerp(int n1, int n2, float t)
	{
		int d = n2 - n1;
		return (float) d*t + n1;
	}

	// Derrived from https://stackoverflow.com/questions/37642168/how-to-convert-quadratic-bezier-curve-code-into-cubic-bezier-curve/37642695#37642695
	Points BezierLerp(int i0, int i1, int i2, int i3) {

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
	   

};

//----------------------------------------------------------------------------


// OpenGL initialization
enum objects { cpID, curveID };
GLuint VAO[2];

GLuint shader;
GLuint vPosition;
GLuint vColor;

// Data 
Points cp;
Points curve;


void
init()
{
	// Load shaders and use the resulting shader program
	shader = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(shader);
	vPosition = glGetAttribLocation(shader, "vPosition");
	vColor = glGetAttribLocation(shader, "vColor");
	   
	// Create Vertex Array Objects
	glGenVertexArrays(2, VAO);
	   
	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);
}


//----------------------------------------------------------------------------

void
display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Load geomerty to GPU			
	cp.extractGeometry().load(VAO[cpID], vPosition, vColor);
	curve.extractGeometry().load(VAO[curveID], vPosition, vColor);
	
	glBindVertexArray(VAO[cpID]);
	{
		glPointSize(10.0f);
		glDrawElements(GL_POINTS, cp.numElements(), GL_UNSIGNED_INT, 0);
	}

	glBindVertexArray(VAO[curveID]);
	{
		glPointSize(6.0f);
		glDrawElements(GL_POINTS, curve.numElements(), GL_UNSIGNED_INT, 0);
	}

	glutSwapBuffers();
}

//----------------------------------------------------------------------------

int mode;
bool reset = false;

int curr = 1;

Points curveSegment;
bool drawing = false;
bool erasing = false;
int tempPoints = 0;
int tempMax = 1 / 0.02;

void
keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 033: // Escape Key
	case 'q': case 'Q':
		exit(EXIT_SUCCESS);
		break;
	case ' ':  // hold
		mode++;
		if (mode > 2) {
			mode = 0;
		}
		break;	
	case 'r':
		//reset = true;

		if (cp.numElements() > 3) {			
			curve = Points();
			curveSegment = cp.lerp();
			drawing = true;
		}

		break;
	}
}


//----------------------------------------------------------------------------

void
mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN && drawing == false && erasing == false) {
		switch (button) {
		case GLUT_LEFT_BUTTON:
		{
			float windowX = -1.0f + x * 2.0f / glutGet(GLUT_WINDOW_WIDTH);
			float windowY = 1.0f + y * 2.0f / -glutGet(GLUT_WINDOW_HEIGHT);
						
			Point p(glm::vec2(windowX, windowY), glm::vec4(0.1, 0.1, 0.1, 1.0));
								
			if (cp.numElements() == 0) { // put extra 1st point
				cp.add(p);
				cp.add(p);
			}
			else if (cp.numElements() == 2) { // 3 points is not enough to make a curve
				cp.add(p);
			}
			else if (cp.numElements() == 3) { // 5 points is enough to make 2 curves
				cp.add(p);
				cp.add(p);

				curveSegment = cp.catmullRomLerp(curr, curr + 1, curr + 2, curr + 3);
				curveSegment.extend(cp.catmullRomLerp(curr - 1, curr, curr + 1, curr + 2));
				cp.pop();

				drawing = true;
				tempPoints = 0;
				tempMax = curveSegment.numElements();
			}
			else if (cp.numElements() > 3) {	// the last point is also a cp point					
				cp.add(p);
				cp.add(p);
				curveSegment = cp.catmullRomLerp(curr, curr + 1, curr + 2, curr + 3);
				cp.pop();
				
				drawing = true;
				tempPoints = 0;
				tempMax = curveSegment.numElements();
			}			

			display();

			//std::cout << "x=" << x << "\ty=" << y << "\n";						
			break;
		}
		case GLUT_RIGHT_BUTTON:
		{
			if (cp.numElements() > 0) {
				if (cp.numElements() > 3) { // remove last curve
					cp.pop();
					erasing = true;
					display();
				}
				else if (cp.numElements() == 3) { // remove last 2 curves
					cp.pop();
					erasing = true;
					display();
				}
				else if (cp.numElements() == 2) { // remove redundant starting point
					cp.pop();
					cp.pop();
					display();
					curr = 1;
				}
			}
			break;
		}
		case GLUT_MIDDLE_BUTTON: break;
		}
	}
}

//----------------------------------------------------------------------------

void
update(void)
{
	if (drawing) {
		if (curveSegment.numElements() > 0) {
			Point p = curveSegment.pop();
			curve.add(p);
			tempPoints++;
			display();
		}
		else {
			drawing = false;
			curr++;
		}
	}

	if (erasing) {
		if (tempPoints > 0 && curve.numElements() > 0)
		{
			curve.pop();
			display();
			tempPoints--;
		}
		else {
			erasing = false;
			curr--;
			if (cp.numElements() == 4) {
				tempPoints = tempMax * 2;
			}
			else {
				tempPoints = tempMax;
			}

		}

	}
}
//----------------------------------------------------------------------------

void
reshape(int width, int height)
{
	glViewport(0, 0, width, height);
}
