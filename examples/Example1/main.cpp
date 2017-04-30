/************************************************
 *
 *             Example One
 *
 *  A very basic OpenGL program that draws a
 *  triangle on the screen.  This program illustrates
 *  the basic code required for an OpenGL program.
 *
 ************************************************/

#include <Windows.h>
#include <gl/glew.h>
#include <gl/glut.h>
#include "shaders.h"
#include <stdio.h>

GLuint program;			// shader programs
GLuint triangleVAO;		// the data to be displayed

/*
 *  The init procedure creates the OpenGL data structures
 *  that contain the triangle geometry, compiles our
 *  shader program and links the shader programs to
 *  the data.
 */

void init() {
	GLuint vbuffer;
	GLuint ibuffer;
	GLint vPosition;
	int vs;
	int fs;

	glGenVertexArrays(1, &triangleVAO);
	glBindVertexArray(triangleVAO);

	GLfloat vertices[3][2] = {	// coordinates of triangle vertices
		{ -0.5, -0.5 },
		{  0.0,  0.5},
		{  0.5, -0.5}
	};

	GLushort indexes[3] = { 0, 1, 2 };	// indexes of triangle vertices

	/*
	 *  load the vertex coordinate data
	 */
	glGenBuffers(1, &vbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	
	/*
	 *  load the vertex indexes
	 */
	glGenBuffers(1, &ibuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexes), indexes, GL_STATIC_DRAW);

	/*
	 *  compile and build the shader program
	 */
	vs = buildShader(GL_VERTEX_SHADER, "example1.vs");
	fs = buildShader(GL_FRAGMENT_SHADER, "example1.fs");
	program = buildProgram(vs,fs,0);

	/*
	 *  link the vertex coordinates to the vPosition
	 *  variable in the vertex program
	 */
	glUseProgram(program);
	vPosition = glGetAttribLocation(program,"vPosition");
	glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);

}

/*
 *  This procedure is called each time the screen needs
 *  to be redisplayed
 */
void displayFunc() {

	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(program);

	glBindVertexArray(triangleVAO);
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, NULL);

	glutSwapBuffers();

}

int main(int argc, char **argv) {

	/*
	 *  initialize glut, set some parameters for
	 *  the application and create the window
	 */
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(320,320);
	glutCreateWindow("Example One");

	/*
	 *  initialize glew
	 */
	GLenum error = glewInit();
	if(error != GLEW_OK) {
		printf("Error starting GLEW: %s\n",glewGetErrorString(error));
		exit(0);
	}

	glutDisplayFunc(displayFunc);

	init();

	glClearColor(1.0,1.0,1.0,1.0);

	glutMainLoop();

}