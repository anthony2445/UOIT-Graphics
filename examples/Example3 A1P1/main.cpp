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

GLuint program;			// shader programs
GLuint triangleVAO;		// the data to be displayed
float angle = 0.0;		// rotation angle and its initial value
int window;				// ID of the window we are using
int faces=1;
float eyex, eyey, eyez;	// eye position
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

	//const int vertexNum = model->nvertex;

	//GLfloat vertices[vertexNum][3] = model->vertices;
	GLfloat * vertices = new GLfloat[model->nvertex * 3];
	for (int i = 0; i < model->nvertex; i++)
	{
		vertices[(i*3) + 0] = model->vertices[i].x;
		vertices[(i*3) + 1] = model->vertices[i].y;
		vertices[(i*3) + 2] = model->vertices[i].z;
	}

	/*GLfloat normals[3][3] = {	// the normal vectors for each vertex
		{0.0, 0.0, 1.0},
		{0.0, 0.0, 1.0},
		{0.0, 0.0, 1.0}
	};*/
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

	//calculating normals
	GLfloat * vertNormals = new GLfloat[model->nface * 3];

	for (int i = 0; i < model->nface; i++)
	{
		vertNormals[(i * 3) + 0] = 0.0;
		vertNormals[(i * 3) + 1] = 0.0;
		vertNormals[(i * 3) + 2] = 0.0;
	}

	GLfloat length = 0;
	GLfloat vertexAx = 0;
	GLfloat vertexAy = 0;
	GLfloat vertexAz = 0;
	GLfloat vertexBx = 0;
	GLfloat vertexBy = 0;
	GLfloat vertexBz = 0;
	GLfloat vertexCx = 0;
	GLfloat vertexCy = 0;
	GLfloat vertexCz = 0;
	GLfloat bsubtractAx = 0;
	GLfloat bsubtractAy = 0;
	GLfloat bsubtractAz = 0;
	GLfloat csubtractAx = 0;
	GLfloat csubtractAy = 0;
	GLfloat csubtractAz = 0;
	GLfloat crossX = 0;
	GLfloat crossY = 0;
	GLfloat crossZ = 0;

	for (int i = 0; i < model->nface; i++)
	{
		vertexAx = vertices[indexes[(i * 3)]];
		vertexAy = vertices[indexes[(i * 3)]+1];
		vertexAz = vertices[indexes[(i * 3)]+2];
		vertexBx = vertices[indexes[(i * 3) + 1]];
		vertexBy = vertices[indexes[(i * 3) + 1] + 1];
		vertexBz = vertices[indexes[(i * 3) + 1] + 2];
		vertexCx = vertices[indexes[(i * 3) + 2]];
		vertexCy = vertices[indexes[(i * 3) + 2] + 1];
		vertexCz = vertices[indexes[(i * 3) + 2] + 2];
		bsubtractAx = vertexBx - vertexAx;
		bsubtractAy = vertexBy - vertexAy;
		bsubtractAz = vertexBz - vertexAz;
		csubtractAx = vertexCx - vertexAx;
		csubtractAy = vertexCy - vertexAy;
		csubtractAz = vertexCz - vertexAz;
		crossX = ((bsubtractAy*csubtractAz) - (csubtractAy*bsubtractAz));
		crossY = ((bsubtractAz*csubtractAx) - (csubtractAz*bsubtractAx));
		crossX = ((bsubtractAx*csubtractAy) - (csubtractAx*bsubtractAy));

		length = sqrt((crossX*crossX)+ (crossY*crossY) + (crossZ*crossZ));
		vertNormals[indexes[(i*3)]] += (crossX/length);
		vertNormals[indexes[(i * 3)+1]] += (crossY / length);
		vertNormals[indexes[(i * 3)+2]] += (crossZ / length);
	}

	/*GLfloat * normals = new GLfloat[model->nface * 3];
	for (int i = 0; i < model->nface; i++)
	{
		normals[(i * 3) + 0] = 0.0;
		normals[(i * 3) + 1] = 0.0;
		normals[(i * 3) + 2] = 1.0;
	}*/

	/*
	 *  load the vertex coordinate data and normal vectors
	 */
	glGenBuffers(1, &vbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
	glBufferData(GL_ARRAY_BUFFER, (model->nvertex*3*sizeof(GLfloat))+ (model->nvertex * 3 * sizeof(GLfloat)), NULL, GL_STATIC_DRAW);
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
	vs = buildShader(GL_VERTEX_SHADER, "example3.vs");
	fs = buildShader(GL_FRAGMENT_SHADER, "example3.fs");
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
	GLint vPosition;
	int colourLoc;

	view = glm::lookAt(glm::vec3(eyex, eyey, eyez),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, 1.0f));

	viewPerspective = projection * view;

	model = glm::rotate(glm::mat4(1.0), angle, glm::vec3(0.0, 1.0, 0.0));

	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(program);
	viewLoc = glGetUniformLocation(program, "viewPerspective");
	glUniformMatrix4fv(viewLoc, 1, 0, glm::value_ptr(viewPerspective));
	modelLoc = glGetUniformLocation(program,"model");
	glUniformMatrix4fv(modelLoc, 1, 0, glm::value_ptr(model));

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

	float ratio = 3.0 * w / h;

	glViewport(-900, -1000, w*4, h*4);

	projection = glm::perspective(45.0f, ratio, 1.0f, 100.0f);

}


/*
*  Called each time a key is pressed on
*  the keyboard.
*/
void keyboardFunc(unsigned char key, int x, int y) {

	switch (key) {
	case 'a':
		eyez -= 0.1;
		break;
	case 'd':
		eyez += 0.1;
		break;
	case 'w':
		eyex += 0.1;
		break;
	case 's':
		eyex -= 0.1;
		break;
	}
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
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
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
	eyey = 6.0;
	eyez = 1.0;

	init();

	glClearColor(1.0,1.0,1.0,1.0);

	glutMainLoop();

}