#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "common.h"
#include "geometry.h"
#include "points.h"

#include <iostream>
#include <vector>

const char* WINDOW_TITLE = "curves";
const double FRAME_RATE_MS = 1000.0 / 60.0;

//----------------------------------------------------------------------------
// OpenGL initialization
enum objects { ControlPointsID, CurvePointsID };
GLuint VAO[2];

GLuint shader;
GLuint vPosition;
GLuint vColor;

// Data
Points cp;
Points curve;

//----------------------------------------------------------------------------
// Called when the state of geometry is changed

void
bind()
{
	// Load current geomerty to GPU			
	cp.extractGeometry().load(VAO[ControlPointsID], vPosition, vColor);
	curve.extractGeometry().load(VAO[CurvePointsID], vPosition, vColor);
}

//----------------------------------------------------------------------------
// Called 1 time (setup)

void
init()
{
	// Load shaders and use the resulting shader program
	shader = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(shader);
	vPosition = glGetAttribLocation(shader, "vPosition");
	vColor = glGetAttribLocation(shader, "vColor");
	   
	// Load geomerty to GPU			
	bind();

	// Create Vertex Array Objects
	glGenVertexArrays(2, VAO);
	   
	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);
}


//----------------------------------------------------------------------------
// Called every frame 

void
display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	   		   
	glBindVertexArray(VAO[ControlPointsID]);
	{
		glPointSize(10.0f);
		glDrawElements(GL_POINTS, cp.numElements(), GL_UNSIGNED_INT, 0);
	}

	glBindVertexArray(VAO[CurvePointsID]);
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

		/*		Geometry geometry = curveSegment.extractGeometry();
				for (float t = 0; t < 1; t += 0.01) {
					geometry.subData(t).load(VAO[CurvePointsID], vPosition, vColor);
					display();
				}*/
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
			bind();

			//std::cout << "x=" << x << "\ty=" << y << "\n";						
			break;
		}
		case GLUT_RIGHT_BUTTON:
		{
			if (cp.numElements() > 0) {
				if (cp.numElements() > 3) { // remove last curve
					cp.pop();
					erasing = true;					
				}
				else if (cp.numElements() == 3) { // remove last 2 curves
					cp.pop();
					erasing = true;					
				}
				else if (cp.numElements() == 2) { // remove redundant starting point
					cp.pop();
					cp.pop();					
					curr = 1;
				}
				bind();
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
			bind();
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
			bind();
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
