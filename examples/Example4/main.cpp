/************************************************
 *
 *             Example Four
 *
 *  A basic OpenGL program that draws a
 *  triangle on the screen in perspective with
 *  simple controil over the eye position.  
 *  This program illustrates the construction of
 *  perspective and viewing transformations.
 *
 ************************************************/

#include <Windows.h>
#include <gl/glew.h>
#include <gl/glut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shaders.h"
#include <stdio.h>
#include "tiny_obj_loader.h"
#include "readply.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;



GLuint program;			// shader programs
GLuint triangleVAO;		// the data to be displayed
GLuint objVAO; // the data to be displayed
int triangles; // number of triangles
int numPoints; //used to hold the number of points found at the beginning of the points file
float angle = 0.0;
int window;

glm::mat4 projection;	// projection matrix
float eyex, eyey, eyez, theta, phi, r, cx, cy, cz;	// eye position

/*
 *  The init procedure creates the OpenGL data structures
 *  that contain the triangle geometry, compiles our
 *  shader program and links the shader programs to
 *  the data.
 */

void init() {

	ifstream myfile("points.txt");
	string line;
	float x, y, z;
	

	getline(myfile, line); //read in first line

	numPoints = atoi(line.c_str());
	cout << numPoints << '\n';

	GLfloat *points = new GLfloat[numPoints * 3];

	/*while (myfile)
	{
		if (!getline(myfile, line)) break;

		istringstream ss(line);
		vector
	}*/

	
	if (myfile.is_open())
	{
		int i = 0;
		while (!myfile.eof())
		{
			myfile >> x >> y >> z;
			points[i * 3] = x;
			points[i * 3 + 1] = y;
			points[i * 3 + 1] = z;
			cout << points[i * 3] << '\n';
			i++;
		}
		myfile.close();
	}

	GLfloat tension = 0.5;
	GLfloat s = 1 / 2 * (1 - tension);
	GLfloat Cmatrix[4][4] = {	
		{ 1.0, 1-(1/s), 1.0, 1.0 },
		{ 1.0,  0.0, 0.0, 0.0 },
		{ 1.0, 1.0, 1.0, 1.0 },
		{ 1.0, 1/s, 2/s, 3/s}
	};

	GLfloat Bmatrix[4][4] = {
		{ 0.0, 1.0, 0.0, 0.0 },
		{ -s,  0.0, -s, 0.0 },
		{ 2*s, s-3, 3-2*s, -s },
		{ -s, 2 - s, s - 2, s }
	};
	GLfloat Umatrix[4] = { 0.0, 0.0, 0.0, 0.0};
	GLfloat px[4] = { 0.0, 0.0, 0.0, 0.0 };
	GLfloat py[4] = { 0.0, 0.0, 0.0, 0.0 };
	GLfloat pz[4] = { 0.0, 0.0, 0.0, 0.0 };
	GLfloat UBxmatrix[4];
	GLfloat UBymatrix[4];
	GLfloat UBzmatrix[4];
	GLfloat *vertArray = new GLfloat[numPoints * 3 * 10];


	//********************************important************************
	for (int i = 0; i < numPoints - 2; i++)
	{
		px[0] = points[i * 3];
		px[1] = points[(i + 1) * 3];
		px[2] = points[(i + 2) * 3];
		px[3] = points[(i + 3) * 3];
		py[0] = points[(i * 3)+1];
		py[1] = points[((i + 1) * 3)+1];
		py[2] = points[((i + 2) * 3)+1];
		py[3] = points[((i + 3) * 3)+1];
		pz[0] = points[(i * 3)+2];
		pz[1] = points[((i + 1) * 3)+2];
		pz[2] = points[((i + 2) * 3)+2];
		pz[3] = points[((i + 3) * 3)+2];
		for (int j = 0; j < 4; j++)
		{
			UBxmatrix[j] = (Bmatrix[0][j]+ Bmatrix[1][j] + Bmatrix[2][j] + Bmatrix[3][j])*px[j];
			UBymatrix[j] = (Bmatrix[0][j] + Bmatrix[1][j] + Bmatrix[2][j] + Bmatrix[3][j])*py[j];
			UBzmatrix[j] = (Bmatrix[0][j] + Bmatrix[1][j] + Bmatrix[2][j] + Bmatrix[3][j])*pz[j];
		}
		
		for (float u = 0.0; u < 1.0; u += 0.1)
		{
			Umatrix[0] = 1.0;
			Umatrix[1] = u;
			Umatrix[2] = (u*u);
			Umatrix[3] = (u*u*u);
			int temp = u * 10;
			vertArray[i * 3 * temp] = 0;
			for (int j = 0; j < 4; j++)
			{
				vertArray[i*3*temp] += UBxmatrix[j] * Umatrix[j]; //x values
				vertArray[(i * 3 * temp)+1] += UBymatrix[j] * Umatrix[j]; //y values
				vertArray[(i * 3 * temp)+2] += UBzmatrix[j] * Umatrix[j]; //z values
			}
			//cout << vertArray[i * 3 * temp] << '\n';
		}

	}


	/*
	*  load the vertex coordinate data
	*/
	/*
	glGenBuffers(1, &vbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
	glBufferData(GL_ARRAY_BUFFER, (nv + nn)* 3 * sizeof(GLfloat), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, nv * 3 * sizeof(GLfloat), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, nv * 3 * sizeof(GLfloat), nn * 3 * sizeof(GLfloat), normals);

	/*
	*  load the vertex indexes
	*/
	/*
	glGenBuffers(1, &ibuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ni * 3 * sizeof(GLushort), indices, GL_STATIC_DRAW);

	/*
	*  compile and build the shader program
	*/
	/*
	vs = buildShader(GL_VERTEX_SHADER, "lab2.vs");
	fs = buildShader(GL_FRAGMENT_SHADER, "lab2.fs");
	program = buildProgram(vs, fs, 0);

	/*
	*  link the vertex coordinates to the vPosition
	*  variable in the vertex program.  Do the same
	*  for the normal vectors.
	*/
	/*
	glUseProgram(program);
	vPosition = glGetAttribLocation(program, "vPosition");
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);
	vNormal = glGetAttribLocation(program, "vNormal");
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, (void*) (nv * 3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(vNormal);
	*/
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

	projection = glm::perspective(45.0f, ratio, 0.1f, 200.0f);
	
}

/*
 *  This procedure is called each time the screen needs
 *  to be redisplayed
 */
void displayFunc() {
	glm::mat4 model; //new
	glm::mat4 view;
	glm::mat4 modelViewPerspective;
	//int viewLoc; //new
	int modelLoc;
	int normalLoc;

	model = glm::rotate(glm::mat4(1.0), angle, glm::vec3(0.0, 1.0, 0.0));

	view = glm::lookAt(glm::vec3(eyex, eyey, eyez),
		glm::vec3(cx, cy, cz),
		glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat3 normal = glm::transpose(glm::inverse(glm::mat3(view*model)));
	modelViewPerspective = projection * view * model;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(program);

	modelLoc = glGetUniformLocation(program, "model");
	glUniformMatrix4fv(modelLoc, 1, 0, glm::value_ptr(modelViewPerspective));
	normalLoc = glGetUniformLocation(program, "normalMat");
	glUniformMatrix3fv(normalLoc, 1, 0, glm::value_ptr(normal));
	glBindVertexArray(objVAO);
	glDrawElements(GL_TRIANGLES, 3*triangles, GL_UNSIGNED_SHORT, NULL);

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

	switch (key) {
	case 'a':
		eyex -= 0.1;
		break;
	case 'd':
		eyex += 0.1;
		break;
	case 'w':
		eyez += 0.1;
		break;
	case 's':
		eyez -= 0.1;
		break;
	}
	////eyex = r*sin(theta)*cos(phi);
	//eyey = r*sin(theta)*sin(phi);
	//eyez = r*cos(theta);
	glutPostRedisplay();

}

int main(int argc, char **argv) {

	/*
	 *  initialize glut, set some parameters for
	 *  the application and create the window
	 */
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(100,50);
	glutInitWindowSize(600,600);
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
	//glutReshapeFunc(changeSize);
	//glutKeyboardFunc(keyboardFunc);

	eyex = 0.0;
	eyey = 0.0;
	eyez = -0.6;

	theta = 1.5;
	phi = 1.5;
	r = 0.3;


	init();

	glClearColor(1.0,1.0,1.0,1.0);

	glEnable(GL_DEPTH_TEST);
	glutMainLoop();

}