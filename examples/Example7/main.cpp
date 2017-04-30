/********************************************
 *
 *             Example7
 *
 *  Simple texture mapping example.  Loosely
 *  based on one of the examples from the
 *  old red book.  Texture is defined in memory
 *
 ******************************************/
#include <Windows.h>
#include <gl/glew.h>
#include <gl/glut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shaders.h"
#include <stdio.h>

GLuint program;			// shader programs
GLuint objVAO;			// the data to be displayed
int triangles;			// number of triangles
int window;
GLuint texName;			// texture name
glm::mat4 projection;	// projection matrix

#define	checkImageWidth 64
#define	checkImageHeight 64
GLubyte checkImage[checkImageWidth][checkImageHeight][4];

/*
 *  Create the simple checkerboard texture
 */
void makeCheckImage(void)
{
    int i, j, c;
    
    for (i = 0; i < checkImageWidth; i++) {
		for (j = 0; j < checkImageHeight; j++) {
			c = ((((i&0x8)==0)^((j&0x8)==0)))*255;
			checkImage[i][j][0] = (GLubyte) c;
			checkImage[i][j][1] = (GLubyte) c;
			checkImage[i][j][2] = (GLubyte) c;
			checkImage[i][j][3] = (GLubyte) 255;
		}
    }
}

/*
 *  Initialize the vertex buffer object and
 *  the texture.
 */
void init(void) { 
	GLuint vbuffer;
	GLuint ibuffer;
	GLuint vs;
	GLuint fs;
	GLuint vPosition;
	GLuint vTexture;

	glGenVertexArrays(1, &objVAO);
	glBindVertexArray(objVAO);

	/*
	 *  Two squares, one straight on, the other
	 *  slanted into the screen.
	 */
	GLfloat vertices[] = {
		 -2.0, -1.0, 0.0,
		 -2.0, 1.0, 0.0,
		  0.0, 1.0, 0.0,
		  0.0, -1.0, 0.0,
		  1.0, -1.0, 0.0,
		  1.0, 1.0, 0.0,
		  2.41421, 1.0, -1.41421,
		  2.41421, -1.0, -1.41421 };

	GLfloat texCoords[] = {
		0.0, 0.0,
		0.0, 1.0,
		1.0, 1.0,
		1.0, 0.0,
		0.0, 0.0,
		0.0, 1.0,
		1.0, 1.0,
		1.0, 0.0};

	GLuint indices[] = {0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4 };

	glGenBuffers(1, &vbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices)+sizeof(texCoords), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(texCoords), texCoords);

	glGenBuffers(1, &ibuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	vs = buildShader(GL_VERTEX_SHADER, "texture.vs");
	fs = buildShader(GL_FRAGMENT_SHADER, "texture.fs");
	program = buildProgram(vs,fs,0);

	glUseProgram(program);
	vPosition = glGetAttribLocation(program,"vPosition");
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);
	vTexture = glGetAttribLocation(program, "vTexture");
	glVertexAttribPointer(vTexture, 2, GL_FLOAT, GL_FALSE, 0, (void*) sizeof(vertices));
	glEnableVertexAttribArray(vTexture);
	triangles = 4;

    glClearColor (0.3, 0.3, 0.3, 1.0);
    glEnable(GL_DEPTH_TEST);

	/*
	 *  Create the texture.
	 */
    makeCheckImage();
    
    glGenTextures(1, &texName);
    glBindTexture(GL_TEXTURE_2D, texName);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, checkImageWidth, 
	checkImageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 
	&checkImage[0][0][0]);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

}

void reshapeFunc(int w, int h) {

    glViewport(0, 0, w, h);
    float ratio = 1.0f * w / h;
    
	projection = glm::perspective(45.0f, ratio, 1.0f,30.0f);
    
    glTranslatef(0.0, 0.0, -3.6);
}


void displayFunc(void) {
	glm::mat4 view;
	int modelViewLoc;

	view = projection * glm::translate(glm::mat4(1.0), glm::vec3(0.0, 0.0, -4.0));
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(program);
	modelViewLoc = glGetUniformLocation(program,"modelView");
	glUniformMatrix4fv(modelViewLoc, 1, 0, glm::value_ptr(view));

    glBindTexture(GL_TEXTURE_2D, texName);
	glBindVertexArray(objVAO);
	glDrawElements(GL_TRIANGLES, 3*triangles, GL_UNSIGNED_INT, NULL);
    
    glutSwapBuffers();
}



int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(500,500);
	window = glutCreateWindow("Example 7 - checkerboard texture");

	/*
	 *  initialize glew
	 */
	GLenum error = glewInit();
	if(error != GLEW_OK) {
		printf("Error starting GLEW: %s\n",glewGetErrorString(error));
		exit(0);
	}

    glutReshapeFunc (reshapeFunc);
    glutDisplayFunc(displayFunc);

	init();

    glutMainLoop();
    return 0;
}

