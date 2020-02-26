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

Points curveSegment;
bool interpolation = false;
bool erasing = false;
int pointsToErase = 0;

void
keyboard(unsigned char key, int x, int y)
{
	if(!interpolation){	
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

			if (cp.numElements() > 3)
			{
				curve = Points();
				curveSegment = cp.lerp(mode);
				interpolation = true;
			}
			break;	
		case 'r':
			if (cp.numElements() > 3)
			{
				curve = Points();
				curveSegment = cp.lerp(mode);
				interpolation = true;
			}
			break;
		}
	}
}


//----------------------------------------------------------------------------

void
mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN && interpolation == false && erasing == false) {
		switch (button) {
		case GLUT_LEFT_BUTTON:
		{
			float windowX = -1.0f + x * 2.0f / glutGet(GLUT_WINDOW_WIDTH);
			float windowY = 1.0f + y * 2.0f / -glutGet(GLUT_WINDOW_HEIGHT);
						
			Point p(glm::vec2(windowX, windowY), glm::vec4(0.1, 0.1, 0.1, 1.0));
							
			std::cout << "X=" << windowX << "\nY=" << windowY << "\n\n";
			curveSegment = cp.lerp(p,mode);				
			if(curveSegment.numElements() > 0)
			{					
				interpolation = true;		
				std::cout << "Interpolating"<< "\n\n";
			}	
			bind();				
			break;
		}
		case GLUT_RIGHT_BUTTON:
		{		
			if (cp.numElements() > 3) {
				pointsToErase = 1.0/0.005; // 1.0 / 0.001
				erasing = true;
				std::cout << "Erasing" << "\n\n";
			}

			if (cp.numElements() > 0) {
				cp.pop();
				std::cout << "Control point removed" << "\n\n";
			}
		
			break;
		}
		case GLUT_MIDDLE_BUTTON: break;
		}
	}
}

//----------------------------------------------------------------------------
// Called every frame 

void
update(void)
{
	int pointsPerUpdate = 35; 

	if (interpolation) {
		int i = 0;
		while (i < pointsPerUpdate)
		{
			if (curveSegment.numElements() > 0 ) {
				curve.add(curveSegment.pop());
			} 
			else {
				interpolation = false;
			} 
			i++;
		}
		bind();			
	}	
	if (erasing) {		
		int i = 0;
		while(i<pointsPerUpdate)
		{			
			if (pointsToErase >= 0)
			{
				curve.pop();		
				pointsToErase--;
			}
			else
			{
				erasing = false;
			}			
			i++;
		}
		bind();		
	}
}
//----------------------------------------------------------------------------

void
reshape(int width, int height)
{
	glViewport(0, 0, width, height);
}
