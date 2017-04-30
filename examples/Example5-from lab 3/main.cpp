/************************************************
 *
 *             Example Five
 *
 *  The stickman program.  This example shows
 *  how a hierarchical model can be constructed
 *  and displayed using OpenGL
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
#include <stack>

using namespace std;

GLuint program;			// shader programs
int window;

glm::mat4 projection;	// projection matrix
float eyex, eyey, eyez;	// eye position
float walk;

struct Master {
	GLuint vao;
	int indices;
	GLuint ibuffer;
};

Master *body;
Master *head;
Master *leg;
Master *arm;

Master *cylinder(double radius, double height, int sides) {
	double *x;
	double *y;
	Master *result;
	GLuint vao;
	double angle;
	double dangle;
	int i;
	GLfloat *vertices;
	GLushort *indices;
	int j;
	int base;
	GLuint vbuffer;
	GLuint ibuffer;
	GLint vPosition;

	result = new Master;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	result->vao = vao;
	result->indices = 3*4*sides;

	x = new double[sides];
	y = new double[sides];
	dangle = 6.28/sides;
	angle = 0.0;
	for(i=0; i<sides; i++) {
		x[i] = radius*cos(angle);
		y[i] = radius*sin(angle);
		angle += dangle;
	}

	vertices = new GLfloat[3*2*(sides+1)];
	j = 0;

	/*  bottom */
	vertices[j++] = 0.0;
	vertices[j++] = 0.0;
	vertices[j++] = 0.0;
	for(i=0; i<sides; i++) {
		vertices[j++] = x[i];
		vertices[j++] = y[i];
		vertices[j++] = 0.0;
	}

	/* top */
	vertices[j++] = 0.0;
	vertices[j++] = 0.0;
	vertices[j++] = height;
	for(i=0; i<sides; i++) {
		vertices[j++] = x[i];
		vertices[j++] = y[i];
		vertices[j++] = height;
	}

	indices = new GLushort[3*4*sides];
	j=0;

	/* bottom */
	for(i=0; i<sides; i++) {
		indices[j++] = 0;
		indices[j++] = i+1;
		indices[j++] = i+2;
	}
	indices[j-1] = 1;

	/* top */
	base = sides+1;
	for(i=0; i<sides; i++) {
		indices[j++] = base;
		indices[j++] = base+i+1;
		indices[j++] = base+i+2;
	}
	indices[j-1] = base+1;

	/* sides */
	for(i=1; i<sides; i++) {
		indices[j++] = i;
		indices[j++] = base+i;
		indices[j++] = i+1;
		indices[j++] = base+i;
		indices[j++] = base+i+1;
		indices[j++] = i+1;
	}
	indices[j++] = sides;
	indices[j++] = base+sides;
	indices[j++] = 1;
	indices[j++] = base+sides;
	indices[j++] = base+1;
	indices[j++] = 1;

	glGenBuffers(1, &vbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
	glBufferData(GL_ARRAY_BUFFER, 3*2*(sides+1)*sizeof(GLfloat), vertices, GL_STATIC_DRAW);
	
	glGenBuffers(1, &ibuffer);
	result->ibuffer = ibuffer;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3*4*sides*sizeof(GLushort), indices, GL_STATIC_DRAW);

	vPosition = glGetAttribLocation(program,"vPosition");
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);

	return(result);
}

/*
 *  The init procedure creates the OpenGL data structures
 *  that contain the triangle geometry, compiles our
 *  shader program and links the shader programs to
 *  the data.
 */

void init() {
	int vs;
	int fs;

	/*
	 *  compile and build the shader program
	 */
	vs = buildShader(GL_VERTEX_SHADER, "example5.vs");
	fs = buildShader(GL_FRAGMENT_SHADER, "example5.fs");
	program = buildProgram(vs,fs,0);
	dumpProgram(program,"example 5");

	glUseProgram(program);

	body = cylinder(0.2,1.5,10);
	leg = cylinder(0.2,0.9,10);
	arm = cylinder(0.15,0.55,10);
	head = cylinder(0.5,0.4,8);
	

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

	projection = glm::perspective(45.0f, ratio, 1.0f, 100.0f);
	
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
	stack<glm::mat4> matrixStack;
	GLint vPosition;
	int colourLoc;

	view = glm::lookAt(glm::vec3(eyex, eyey, eyez),
					glm::vec3(0.0f, 0.0f, 1.0f),
					glm::vec3(0.0f, 0.0f, 1.0f));
			
	viewPerspective = projection * view;
	model = glm::mat4(1.0);

	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(program);
	viewLoc = glGetUniformLocation(program,"viewPerspective");
	glUniformMatrix4fv(viewLoc, 1, 0, glm::value_ptr(viewPerspective));
	modelLoc = glGetUniformLocation(program,"model");
	colourLoc = glGetUniformLocation(program,"colour");
	vPosition = glGetAttribLocation(program,"vPosition");

	/* body colour - green */

	glUniform4f(colourLoc, 0.0, 1.0, 0.0, 1.0);

	/* draw body */
	glBindVertexArray(body->vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, body->ibuffer);
	glUniformMatrix4fv(modelLoc, 1, 0, glm::value_ptr(model));
	glDrawElements(GL_TRIANGLES, body->indices, GL_UNSIGNED_SHORT, NULL);

	/* draw right leg */
	matrixStack.push(model);
	model = glm::rotate(model, walk, glm::vec3(1.0, 0.0, 0.0));
	model = glm::rotate(model, -2.3F, glm::vec3(0.0, 1.0, 0.0));
	glUniformMatrix4fv(modelLoc, 1, 0, glm::value_ptr(model));
	glBindVertexArray(leg->vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, leg->ibuffer);
	glDrawElements(GL_TRIANGLES, leg->indices, GL_UNSIGNED_SHORT, NULL);
	model = matrixStack.top();
	matrixStack.pop();

	/* draw left leg */
	matrixStack.push(model);
	model = glm::rotate(model, -walk, glm::vec3(1.0, 0.0, 0.0));
	model = glm::rotate(model, 2.3f, glm::vec3(0.0, 1.0, 0.0));
	glUniformMatrix4fv(modelLoc, 1, 0, glm::value_ptr(model));
	glBindVertexArray(leg->vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, leg->ibuffer);
	glDrawElements(GL_TRIANGLES, leg->indices, GL_UNSIGNED_SHORT, NULL);
	model = matrixStack.top();
	matrixStack.pop();

	/* draw right arm */
	matrixStack.push(model);
	model = glm::translate(model,glm::vec3(-0.1, 0.0, 1.0));
	model = glm::rotate(model, -walk, glm::vec3(1.0, 0.0, 0.0));
	model = glm::rotate(model, -1.0f, glm::vec3(0.0, 1.0, 0.0));
	glUniformMatrix4fv(modelLoc, 1, 0, glm::value_ptr(model));
	glBindVertexArray(arm->vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, arm->ibuffer);
	glDrawElements(GL_TRIANGLES, arm->indices, GL_UNSIGNED_SHORT, NULL);
	model = matrixStack.top();
	matrixStack.pop();

	/* draw right arm */
	matrixStack.push(model);
	model = glm::translate(model,glm::vec3(0.1, 0.0, 1.0));
	model = glm::rotate(model, 1.0f, glm::vec3(0.0, 1.0, 0.0));
	glUniformMatrix4fv(modelLoc, 1, 0, glm::value_ptr(model));
	glBindVertexArray(arm->vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, arm->ibuffer);
	glDrawElements(GL_TRIANGLES, arm->indices, GL_UNSIGNED_SHORT, NULL);
	model = matrixStack.top();
	matrixStack.pop();

	/* head colour - yellow */

	glUniform4f(colourLoc, 1.0, 1.0, 0.0, 1.0);

	/* draw head */
	matrixStack.push(model);
	model = glm::translate(model,glm::vec3(0.0, 0.2, 2.0));
	model = glm::rotate(model, 1.57f, glm::vec3(1.0, 0.0, 0.0));
	glUniformMatrix4fv(modelLoc, 1, 0, glm::value_ptr(model));
	glBindVertexArray(head->vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, head->ibuffer);
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);
	glDrawElements(GL_TRIANGLES, head->indices, GL_UNSIGNED_SHORT, NULL);
	model = matrixStack.top();
	matrixStack.pop();

	glutSwapBuffers();

}

/*
 *  Called each time a key is pressed on
 *  the keyboard.
 */
void keyboardFunc(unsigned char key, int x, int y) {

	switch(key) {
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


void idleFunc() {
	static float rate = 0.005;
	static float dwalk = 0.005;
	glutSetWindow(window);
	walk += dwalk;
	if (walk > 1.0)
		dwalk = -rate;
	if (walk < -1.0)
		dwalk = rate;
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
	glutInitWindowSize(500,500);
	window = glutCreateWindow("Example Five");

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