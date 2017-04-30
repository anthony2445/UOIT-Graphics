/************************************************
 *
 *             Example Four
 *
 *  A basic OpenGL program that draws a
 *  triangle on the screen in perspective with
 *  simple control over the eye position.
 *  This program illustrates the construction of
 *  perspective and viewing transformations.
 *
 ************************************************/

#include <fstream>
#include <GL/glew.h>
#include <GL/glut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>
#include "Shaders.h"
#include <stdio.h>
#include <algorithm>

#include "tiny_obj_loader.h"
#include "readply.h"
#include <iostream>
//#include <math>

using namespace std;

GLuint objVAO;   // VAO for line
GLuint pointVAO; // VAO for points
//int n_points;    // number of points
//int n_lines;     // number of lines
vector<int> n_lines;
vector<int> n_points;

GLuint program;         // shader programs
float angle = 0.0;
int window;
float cx, cy, cz, r, theta, phi;

glm::mat4 projection;   // projection matrix
float eyex, eyey, eyez; // eye position
int np = 0;
int triangles = 0;

float TENSION = 0;

/*
 *  The init procedure creates the OpenGL data structures
 *  that contain the triangle geometry, compiles our
 *  shader program and links the shader programs to
 *  the data.
 */

bool first = true;

void init()
{
    // allow calling init() again to change tension
    if (first)
    {
        glGenVertexArrays(1, &objVAO);
        glGenVertexArrays(1, &pointVAO);
        first = false;
    }
    glBindVertexArray(objVAO);

    int num_samples = 10;
    ifstream pi("ptrace20.txt"); //ptrace20.txt
	vector<vector<glm::vec3>> totalSamples;
	vector<vector<glm::vec3>> totalPoints;
	//int np = 0;
	int numPoints = 0;
    int count;
	float xmin, ymin, zmin;
	float xmax, ymax, zmax;
	int idk, idk2;
	pi >> idk >> idk2;
	pi >> xmin >> xmax;
	pi >> ymin >> ymax;
	pi >> zmin >> zmax;
	cx = (xmin + xmax) / 2.0f;
	cy = (ymin + ymax) / 2.0f;
	cz = (zmin + zmax) / 2.0f;
	for (int k = 0; k < 503; k++) //k<idk
	{
		vector<glm::vec3> points;
		pi >> count;
		n_lines.push_back(count);
		// read in the points
		float x1, y1, z1;
		pi >> x1 >> y1 >> z1;
		for (int i = 0; i < count - 1; i++)
		{
			float x2, y2, z2;
			pi >> x2 >> y2 >> z2;
			glm::vec3 point1(x2 - x1, y2 - y1, z2 - z1);
			float minimum = min(point1.x, point1.y);
			minimum = min(minimum, point1.z);
			glm::vec3 point2(0.0, 0.0, 0.0);
			if (minimum == point1.x)
			{
				glm::vec3 point2(point1.x, -point1.z, point1.y);
			}
			else if (minimum == point1.y)
			{
				glm::vec3 point2(-point1.z, point1.y, point1.x);
			}
			else if (minimum == point1.z)
			{
				glm::vec3 point2(-point1.y, point1.x, point1.z);
			}
			glm::vec3 firstVector(((point1.y*point2.z) - (point1.z*point2.y)), ((point1.z*point2.x) - (point1.x*point2.z)), ((point1.x*point2.y) - (point1.y*point2.x)));
			//glm::vec3 firstVector(glm::cross(point1, point2));
			glm::vec3 secondVector(((point1.y*firstVector.z) - (point1.z*firstVector.y)), ((point1.z*firstVector.x) - (point1.x*firstVector.z)), ((point1.x*firstVector.y) - (point1.y*firstVector.x)));

			points.push_back(point1 + firstVector + secondVector);
			points.push_back(point1 + firstVector - secondVector);
			points.push_back(point1 - firstVector - secondVector);
			points.push_back(point1 - firstVector + secondVector);
			numPoints++;
			numPoints++;
			numPoints++;
			numPoints++;
		}
		totalPoints.push_back(points);
		vector<glm::vec3> samples;

		for (int i = 0; i < points.size(); ++i)
		{
			//glm::vec3 res(0, 0, 0);
			//res += points[i] - points[i - 1];
			samples.push_back(points[i]);
			np++;
		}
		totalSamples.push_back(samples);
	}
	cout << "Got past reading" << endl;

    GLuint vbuffer;
    GLuint ibuffer;
    GLint vPosition;
    GLint vNormal;
    int vs;
    int fs;
	int offset=0;
	//cout << numPoints << endl;
	GLfloat * vertices = new GLfloat[numPoints * 3];
	GLushort * indices = new GLushort[np * 12];
	int o = 0;
	int q = 0;
	//cout << n_lines[0] << n_lines[1] << endl;
	//cout << "it worked" << endl;
	for (int k = 0; k < totalPoints.size(); k++)
	{
		int npp = n_lines[k]*4;

		for (int i = 0; i < npp-4; i++)
		{
			//cout << i / 4 << endl;
			//vertex
			vertices[o++] = totalSamples[k][i].x;
			vertices[o++] = totalSamples[k][i].y;
			vertices[o++] = totalSamples[k][i].z;
		}
		//cout << "got out" << endl;
		
	}
	int c = 0;
	int c2 = 0;
	for (int k = 0; k < totalPoints.size(); k++)
	{
		int npp = n_lines[k] * 4;

		for (int i = 0; i < npp-1; i=i+24)
		{
			for (int j = 0; j < 4; j++) {
				//triangle 1
				indices[c + (j * 6)] = c2 + j;
				indices[c + 1 + (j * 6)] = c2 + 1 + j;
				indices[c + 2 + (j * 6)] = c2 + 4 + j;
				triangles++;
				//triangle 2
				indices[c + 3 + (j * 6)] = c2 + 4 + j;
				indices[c + 4 + (j * 6)] = c2 + 5 + j;
				indices[c + 5 + (j * 6)] = c2 + 1 + j;
				triangles++;
			}
			c = c + 24;
			c2 = c2 + 4;
		}
		//cout << k << endl;
	}
	//cout << "added vertices and indices" << endl;


	/*
		*  load the vertex coordinate data
		*/
	glGenBuffers(1, &vbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
	glBufferData(GL_ARRAY_BUFFER, np * 3 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

	/*
		*  load the vertex indexes
		*/
	glGenBuffers(1, &ibuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, np * sizeof(GLushort), indices, GL_STATIC_DRAW);

	/*
		*  compile and build the shader program
		*/
	vs = buildShader(GL_VERTEX_SHADER, "a1q2.vs");
	fs = buildShader(GL_FRAGMENT_SHADER, "a1q2.fs");
	program = buildProgram(vs, fs, 0);

	/*
		*  link the vertex coordinates to the vPosition
		*  variable in the vertex program.  Do the same
		*  for the normal vectors.
		*/
	glUseProgram(program);
	vPosition = glGetAttribLocation(program, "vPosition");
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);

	

}
/*
 *  Executed each time the window is resized,
 *  usually once at the start of the program.
 */
void changeSize(int w, int h) {

    // Prevent a divide by zero, when window is too short
    // (you cant make a window of zero width).

    if(h == 0)
        h = 1;

    float ratio = 1.0 * w / h;

    glViewport(0, 0, w, h);

    projection = glm::perspective(45.0f, ratio, 0.1f, 10000.0f);

}

/*
 *  This procedure is called each time the screen needs
 *  to be redisplayed
 */
void displayFunc() {
    glLineWidth(2);
    glPointSize(5);
    glm::mat4 view;
    glm::mat4 modelViewPerspective;
    int modelLoc;
    int normalLoc;
	int countLines=0;

    view = glm::lookAt(glm::vec3(eyex, eyey, eyez),
                       glm::vec3(cx, cy, cz),
                       glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat3 normal = glm::transpose(glm::inverse(glm::mat3(view)));

    modelViewPerspective = projection * view;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(program);
    modelLoc = glGetUniformLocation(program,"model");
    glUniformMatrix4fv(modelLoc, 1, 0, glm::value_ptr(modelViewPerspective));
    normalLoc = glGetUniformLocation(program,"normalMat");
    glUniformMatrix3fv(normalLoc, 1, 0, glm::value_ptr(normal));

    // color based on tension
    int colorLoc = glGetUniformLocation(program,"color");
    glUniform3f(colorLoc, 1.0, 0.6 - (0.6 * TENSION), 0.0);

    // draw line
	//for loop null= offset, n_lines = lines in that one
	glBindVertexArray(objVAO);
	//glDrawElements(GL_LINE_STRIP, 10000, GL_UNSIGNED_SHORT, (void*)0);
	//for (int i = 0; i < n_lines.size(); i++)
		//glDrawElements(GL_LINE_STRIP, n_lines[i], GL_UNSIGNED_SHORT, (GLvoid*)(countLines * sizeof(GLushort)));
		glDrawElements(GL_TRIANGLES, triangles, GL_UNSIGNED_SHORT, NULL);

    // yellow points
    glUniform3f(colorLoc, 1.0, 1.0, 0.0);

    glutSwapBuffers();

}

/*
 *  Update the value of angle on each update
 */
void idleFunc() {

    glutSetWindow(window);
    angle = angle + 0.001;
    glutPostRedisplay();

}

/*
 *  Called each time a key is pressed on
 *  the keyboard.
 */
void keyboardFunc(unsigned char key, int x, int y) {
    switch(key) {
      case 'a':
          phi-= 0.1;
          break;
      case 'd':
          phi += 0.1;
          break;
      case 'w':
          theta += 0.1;
          break;
      case 's':
          theta -= 0.1;
          break;
      case '-':
          TENSION = max(0.0, TENSION - 0.1);
          init();
          cout << "Tension: " << TENSION << endl;
          break;
      case '+':
          TENSION = min(1.0, TENSION + 0.1);
          init();
          cout << "Tension: " << TENSION << endl;
          break;
    }

    eyex = r*sin(theta)*cos(phi) +cx;
	eyez = 1500;//r*sin(theta)*sin(phi) +1000;
    eyey = r*cos(theta) +cy;

    glutPostRedisplay();
}


int main(int argc, char **argv) {

    /*
     *  initialize glut, set some parameters for
     *  the application and create the window
     */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowPosition(100,100);
    glutInitWindowSize(320,320);
    window = glutCreateWindow("Example Four");

    /*
     *  initialize glew
     */
    GLenum error = glewInit();
    if(error != GLEW_OK) {
        printf("Error starting GLEW: %s\n",glewGetErrorString(error));
        exit(0);
    }

    glutDisplayFunc(displayFunc);
    glutIdleFunc(idleFunc);
    glutReshapeFunc(changeSize);
    glutKeyboardFunc(keyboardFunc);

    theta = 1.5;
    phi = 1.5;
    r = 200;
    eyex = r*sin(theta)*cos(phi);
    eyez = r*sin(theta)*sin(phi);
    eyey = r*cos(theta);

    init();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POINT_SMOOTH);
    glClearColor(0.3,0.3,0.3,1.0);

    glutMainLoop();

}
