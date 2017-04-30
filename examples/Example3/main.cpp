/************************************************
 *
 *             Example Three
 *
 *  A  basic OpenGL program that draws a
 *  triangle on the screen with colour computed using
 *  a simple diffuse light model.  This program illustrates
 *  the addition of normal vectors to the vertex array.
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
#include "readply.h"
#include <iostream>

GLuint program;			// shader programs
GLuint triangleVAO;		// the data to be displayed
float angle = 0.0;		// rotation angle and its initial value
int window;				// ID of the window we are using
int faces=1;
float eyex, eyey, eyez, theta, phi, r, cx, cy, cz;	// eye position
glm::mat4 projection;	// projection matrix

/*
 *  The init procedure creates the OpenGL data structures
 *  that contain the triangle geometry, compiles our
 *  shader program and links the shader programs to
 *  the data.
 */

void init() {

	ply_model* model = readply("bunny.ply");

	GLuint vbuffer;
	GLuint ibuffer;
	GLint vPosition;
	GLint vNormal;
	int vs;
	int fs;

	glGenVertexArrays(1, &triangleVAO);
	glBindVertexArray(triangleVAO);

	GLfloat * vertNormals = new GLfloat[model->nvertex * 3];
	GLfloat * vertices = new GLfloat[model->nvertex * 3];
	for (int i = 0; i < model->nvertex; i++)
	{
		vertices[(i*3) + 0] = model->vertices[i].x;
		vertices[(i*3) + 1] = model->vertices[i].y;
		vertices[(i*3) + 2] = model->vertices[i].z;
		vertNormals[(i * 3) + 0] = 0.0;
		vertNormals[(i * 3) + 1] = 0.0;
		vertNormals[(i * 3) + 2] = 0.0;
	}
	/*
	*  Find the range of the x, y and z
	*  coordinates.
	*/
	int xmin, ymin, zmin, xmax, ymax, zmax;
	xmin = ymin = zmin = 1000000.0;
	xmax = ymax = zmax = -1000000.0;
	for (int i = 0; i<model->nvertex / 3; i++) {
		if (vertices[3 * i] < xmin)
			xmin = vertices[3 * i];
		if (vertices[3 * i] > xmax)
			xmax = vertices[3 * i];
		if (vertices[3 * i + 1] < ymin)
			ymin = vertices[3 * i + 1];
		if (vertices[3 * i + 1] > ymax)
			ymax = vertices[3 * i + 1];
		if (vertices[3 * i + 2] < zmin)
			zmin = vertices[3 * i + 2];
		if (vertices[3 * i + 2] > zmax)
			zmax = vertices[3 * i + 2];
	}
	/* compute center and print range */
	cx = (xmin + xmax) / 2.0f;
	cy = (ymin + ymax) / 2.0f;
	cz = (zmin + zmax) / 2.0f;


	faces = model->nface;


	//GLushort indexes[3] = { 0, 1, 2 };	// indexes of triangle vertices
	GLushort * indexes = new GLushort[3 * model->nface];
	for (int i = 0; i < model->nface; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			indexes[(i*3) + j] = model->faces[i].vertices[j];
		}
	}


	GLfloat length = 0;
	GLfloat vertex[3][3] = { {0,0,0}, {0,0,0}, {0,0,0} };
	/*GLfloat bsubtractAx = 0;
	GLfloat bsubtractAy = 0;
	GLfloat bsubtractAz = 0;
	GLfloat csubtractAx = 0;
	GLfloat csubtractAy = 0;
	GLfloat csubtractAz = 0;
	GLfloat crossX = 0;
	GLfloat crossY = 0;
	GLfloat crossZ = 0;*/

	for (int i = 0; i < model->nvertex; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				vertex[j][k] = vertices[indexes[(i * 3) + j]+k];
			}
		}
			/*bsubtractAx = vertex[1][0] - vertex[0][0];
			bsubtractAy = vertex[1][1] - vertex[0][1];
			bsubtractAz = vertex[1][2] - vertex[0][2];
			csubtractAx = vertex[2][0] - vertex[0][0];
			csubtractAy = vertex[2][1] - vertex[0][1];
			csubtractAz = vertex[2][2] - vertex[0][2];*/

		glm::vec3 u = glm::vec3(vertex[1][0] - vertex[0][0], vertex[1][1] - vertex[0][1], vertex[1][2] - vertex[0][2]);
		glm::vec3 v = glm::vec3(vertex[3][0] - vertex[0][0], vertex[3][1] - vertex[0][1], vertex[3][2] - vertex[0][2]);
			/*crossX = ((bsubtractAy*csubtractAz) - (csubtractAy*bsubtractAz));
			crossY = ((bsubtractAz*csubtractAx) - (csubtractAz*bsubtractAx));
			crossX = ((bsubtractAx*csubtractAy) - (csubtractAx*bsubtractAy));*/
		glm::vec3 crossProd = glm::normalize(glm::cross(u, v));
			//length = sqrt((crossX*crossX) + (crossY*crossY) + (crossZ*crossZ));
			//vertNormals[indexes[(i * 3) + (i % 3)] + j]=

			vertNormals[indexes[(i * 3)]] += crossProd.x; //x component
			vertNormals[indexes[(i * 3)]+1] += crossProd.y; //y component
			vertNormals[indexes[(i * 3)]+2] += crossProd.z; //z component
			vertNormals[indexes[(i * 3)+1]] += crossProd.x; //x component
			vertNormals[indexes[(i * 3)+1] + 1] += crossProd.y; //y component
			vertNormals[indexes[(i * 3)+1] + 2] += crossProd.z; //z component
			vertNormals[indexes[(i * 3)+2]] += crossProd.x; //x component
			vertNormals[indexes[(i * 3)+2] + 1] += crossProd.y; //y component
			vertNormals[indexes[(i * 3)+2] + 2] += crossProd.z; //z component
			//std::cout << "loop" + i;
	}
	for (int i = 0; i < model->nvertex; i++) {
		glm::vec3 n = glm::normalize(glm::vec3(vertNormals[i * 3], vertNormals[(i * 3) + 1], vertNormals[(i * 3) + 2]));
		vertNormals[i * 3] = n.x;
		vertNormals[(i * 3) + 1] = n.y;
		vertNormals[(i * 3) + 2] = n.z;
	}


	/*
	 *  load the vertex coordinate data and normal vectors
	 */
	glGenBuffers(1, &vbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
	glBufferData(GL_ARRAY_BUFFER, (model->nvertex * 3)*2*sizeof(GLfloat), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, (model->nvertex * 3 * sizeof(GLfloat)), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, (model->nvertex * 3 * sizeof(GLfloat)), (model->nvertex * 3 * sizeof(GLfloat)), vertNormals);
	
	/*
	 *  load the vertex indexes
	 */
	glGenBuffers(1, &ibuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (model->nface * 3 * sizeof(GLushort)), indexes, GL_STATIC_DRAW);

	/*
	 *  compile and build the shader program
	 */
	vs = buildShader(GL_VERTEX_SHADER, "lab2.vs");
	fs = buildShader(GL_FRAGMENT_SHADER, "lab2.fs");
	program = buildProgram(vs,fs,0);

	/*
	 *  link the vertex coordinates to the vPosition
	 *  variable in the vertex program and the normal
	 *  vectors to the vNormal variable in the
	 *  vertext program.
	 */
	glUseProgram(program);
	vPosition = glGetAttribLocation(program,"vPosition");
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);
	vNormal = glGetAttribLocation(program, "vNormal");
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, (void*)(model->nvertex * 3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(vNormal);

}

/*
 *  This procedure is called each time the screen needs
 *  to be redisplayed
 */
void displayFunc() {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 viewPerspective;
	int viewLoc;
	int modelLoc;
	int normalLoc;
	GLint vPosition;
	int colourLoc;

	view = glm::lookAt(glm::vec3(eyex, eyey, eyez),
		glm::vec3(cx, cy, cz),
		glm::vec3(0.0f, 0.0f, 1.0f));

	glm::mat3 normal = glm::transpose(glm::inverse(glm::mat3(view)));


	viewPerspective = projection * view;

	//model = glm::rotate(glm::mat4(1.0), angle, glm::vec3(eyex, eyey, eyez));
	model = glm::rotate(glm::mat4(1.0), angle, glm::vec3(0.0, 1.0, 0.0));

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(program);
	viewLoc = glGetUniformLocation(program, "viewPerspective");
	glUniformMatrix4fv(viewLoc, 1, 0, glm::value_ptr(viewPerspective));
	modelLoc = glGetUniformLocation(program,"model");
	glUniformMatrix4fv(modelLoc, 1, 0, glm::value_ptr(model));
	normalLoc = glGetUniformLocation(program, "normalMat");
	glUniformMatrix3fv(normalLoc, 1, 0, glm::value_ptr(normal));


	glBindVertexArray(triangleVAO);
	glDrawElements(GL_TRIANGLES, (3*faces), GL_UNSIGNED_SHORT, NULL);

	glutSwapBuffers();

}

/*
*  Executed each time the window is resized,
*  usually once at the start of the program.
*/
void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).

	if (h == 0)
		h = 1;

	float ratio = 1.0 * w / h;

	glViewport(cx, cy, w, h);

	projection = glm::perspective(45.0f, ratio, 20.0f, 100.0f);

}


/*
*  Called each time a key is pressed on
*  the keyboard.
*/
void keyboardFunc(unsigned char key, int x, int y) {

	switch (key) {
	case 'a':
		phi -= 0.1;
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
	}
	//eyex = r*sin(theta)*cos(phi);
	//eyey = r*sin(theta)*sin(phi);
	//eyez = r*cos(theta);
	glutPostRedisplay();

}

/*
 *  Update the value of angle on each update
 */
void idleFunc() {

	glutSetWindow(window);
	angle = angle + 0.001;
	glutPostRedisplay();

}

int main(int argc, char **argv) {

	/*
	 *  initialize glut, set some parameters for
	 *  the application and create the window
	 */
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glEnable(GL_DEPTH_TEST);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(600,600);
	window = glutCreateWindow("Example Three");

	/*
	 *  initialize glew
	 */
	GLenum error = glewInit();
	if(error != GLEW_OK) {
		printf("Error starting GLEW: %s\n",glewGetErrorString(error));
		exit(0);
	}

	glutDisplayFunc(displayFunc);
	glutReshapeFunc(changeSize);
	glutKeyboardFunc(keyboardFunc);
	glutIdleFunc(idleFunc);

	eyex = 0.0;
	eyey = 1000.0;
	eyez = 0.0;

	theta = 1.5;
	phi = 1.5;
	r = 0.3;

	init();

	glClearColor(1.0,1.0,1.0,1.0);

	glutMainLoop();

}