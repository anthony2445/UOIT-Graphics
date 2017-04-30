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



GLuint program;			// shader programs
GLuint triangleVAO;		// the data to be displayed
GLuint objVAO; // the data to be displayed
int triangles; // number of triangles
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

	ply_model* model = readply("bunny.ply");

	GLuint vbuffer;
	GLuint ibuffer;
	GLint vPosition;
	GLint vNormal;
	int vs;
	int fs;
	GLfloat *vertices = new GLfloat[model->nvertex * 3];
	GLfloat *normals = new GLfloat[model->nvertex * 3];
	GLushort *indices = new GLushort[model->nface * 3];
	//std::vector<tinyobj::shape_t> shapes;
	//std::vector<tinyobj::material_t> materials;
	int nv;
	int nn;
	int ni;
	int i;
	float xmin, ymin, zmin;
	float xmax, ymax, zmax;

	glGenVertexArrays(1, &objVAO);
	glBindVertexArray(objVAO);

	/*  Retrieve the vertex coordinate data */

	nv = model->nvertex;
	for (i = 0; i<nv; i++) {
		vertices[(i * 3) + 0] = model->vertices[i].x;
		vertices[(i * 3) + 1] = model->vertices[i].y;
		vertices[(i * 3) + 2] = model->vertices[i].z;
		normals[(i * 3) + 0] = 0.0; //set normals to 0
		normals[(i * 3) + 1] = 0.0;
		normals[(i * 3) + 2] = 0.0;
	}

	/*
	*  Find the range of the x, y and z
	*  coordinates.
	*/
	xmin = ymin = zmin = 1000000.0;
	xmax = ymax = zmax = -1000000.0;
	for (i = 0; i<nv; i++) {
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
	printf("X range: %f %f\n", xmin, xmax);
	printf("Y range: %f %f\n", ymin, ymax);
	printf("Z range: %f %f\n", zmin, zmax);
	printf("center: %f %f %f\n", cx, cy, cz);


	/*  Retrieve the triangle indices */

	ni = model->nface;
	triangles = ni;
	//indices = new GLushort[ni];
	for (i = 0; i<ni; i++) {
		for (int j = 0; j < 3; j++)
		{
			indices[(i*3)+j] = model->faces[i].vertices[j];
		}
	}

	nn = model->nvertex;
	for (int i = 0; i < model->nface; i++) {
		ply_vertex vert1 = model->vertices[indices[i * 3]];
		ply_vertex vert2 = model->vertices[indices[(i * 3) + 1]];
		ply_vertex vert3 = model->vertices[indices[(i * 3) + 2]];
		glm::vec3 u = glm::vec3(vert2.x - vert1.x, vert2.y - vert1.y, vert2.z - vert1.z);
		glm::vec3 v = glm::vec3(vert3.x - vert1.x, vert3.y - vert1.y, vert3.z - vert1.z);
		glm::vec3 crossProd = glm::normalize(glm::cross(u, v));

		normals[indices[(i * 3)] * 3] += crossProd.x;
		normals[(indices[(i * 3)] * 3)+ 1] += crossProd.y;
		normals[(indices[(i * 3)] * 3) + 2] += crossProd.z;
		normals[indices[(i * 3) + 1] * 3] += crossProd.x;
		normals[(indices[(i * 3) + 1] * 3) + 1] += crossProd.y;
		normals[(indices[(i * 3) + 1] * 3) + 2] += crossProd.z;
		normals[indices[(i * 3) + 2] * 3] += crossProd.x;
		normals[(indices[(i * 3) + 2] * 3) + 1] += crossProd.y;
		normals[(indices[(i * 3) + 2] * 3) + 2] += crossProd.z;
	}
	for (int i = 0; i < model->nvertex; ++i) {
		glm::vec3 n = glm::normalize(glm::vec3(normals[i*3], normals[(i*3) + 1], normals[(i*3) + 2]));
		normals[i*3] = n.x;
		normals[(i*3) + 1] = n.y;
		normals[(i*3) + 2] = n.z;
	}


	/*
	*  load the vertex coordinate data
	*/
	glGenBuffers(1, &vbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
	glBufferData(GL_ARRAY_BUFFER, (nv + nn)* 3 * sizeof(GLfloat), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, nv * 3 * sizeof(GLfloat), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, nv * 3 * sizeof(GLfloat), nn * 3 * sizeof(GLfloat), normals);

	/*
	*  load the vertex indexes
	*/
	glGenBuffers(1, &ibuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ni * 3 * sizeof(GLushort), indices, GL_STATIC_DRAW);

	/*
	*  compile and build the shader program
	*/
	vs = buildShader(GL_VERTEX_SHADER, "lab2.vs");
	fs = buildShader(GL_FRAGMENT_SHADER, "lab2.fs");
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
	vNormal = glGetAttribLocation(program, "vNormal");
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, (void*) (nv * 3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(vNormal);

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
	glutReshapeFunc(changeSize);
	glutKeyboardFunc(keyboardFunc);

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