/************************************************
 *
 *             Example 8
 *
 *  This example uses the vase model plus
 *  a texture map loaded from a file to
 *  illustrate the basic process of texture
 *  mapping.
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
#include <iostream>
#include <FreeImage.h>

GLuint program;			// shader programs
GLuint objVAO;			// the data to be displayed
int triangles;			// number of triangles
int window;

double theta, phi;
double r;

float cx, cy, cz;

glm::mat4 projection;	// projection matrix
float eyex, eyey, eyez;	// eye position

#define	checkImageWidth 64
#define	checkImageHeight 64
GLubyte checkImage[checkImageWidth][checkImageHeight][4];
GLuint texName;			// texture name

struct textureStruct {
	int height;
	int width;
	int bytes;
	unsigned char *data;
};

textureStruct* loadImage(char *filename) {
	int i,j;
	FIBITMAP *bitmap;
	BYTE *bits;
	int width;
	int height;
	int bytes;
	unsigned char *data;
	textureStruct *result;
	int k;

	result = new textureStruct();

	bitmap = FreeImage_Load(FIF_JPEG, filename, JPEG_DEFAULT);
	height = FreeImage_GetHeight(bitmap);
	width = FreeImage_GetWidth(bitmap);
	bytes = FreeImage_GetBPP(bitmap)/8;
	printf("image size: %d %d %d\n",width, height, bytes);
	data = new unsigned char[width*height*bytes];
	result->height = height;
	result->width = width;
	result->bytes = bytes;
	result->data = data;

	k = 0;
	for(j=0; j<height; j++) {
		bits = FreeImage_GetScanLine(bitmap,j);
			for(i=0; i<width; i++) {
				data[k++] = bits[FI_RGBA_RED];
				data[k++] = bits[FI_RGBA_GREEN];
				data[k++] = bits[FI_RGBA_BLUE];
				bits += 3;
			}
	}
	FreeImage_Unload(bitmap);
	return(result);
}


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
 *  The init procedure creates the OpenGL data structures
 *  that contain the triangle geometry, compiles our
 *  shader program and links the shader programs to
 *  the data.
 */

void init() {
	GLuint vbuffer;
	GLuint ibuffer;
	GLint vPosition;
	GLint vNormal;
	GLint vTexcoord;
	int vs;
	int fs;
	GLfloat *vertices;
	GLfloat *normals;
	GLfloat *texcoords;
	GLuint *indices;
	std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
	int nv;
	int nn;
	int nt;
	int ni;
	int i;
	float xmin, ymin, zmin;
	float xmax, ymax, zmax;
	textureStruct *texture;

	glGenVertexArrays(1, &objVAO);
	glBindVertexArray(objVAO);

	/*  Load the obj file */

	std::string err = tinyobj::LoadObj(shapes, materials, "vase.obj", 0);

    if (!err.empty()) {
		std::cerr << err << std::endl;
		return;
    }

	/*  Retrieve the vertex coordinate data */

	nv = shapes[0].mesh.positions.size();
	vertices = new GLfloat[nv];
	for(i=0; i<nv; i++) {
		vertices[i] = shapes[0].mesh.positions[i];
	}

	/*
	 *  Find the range of the x, y and z
	 *  coordinates.
	 */
	xmin = ymin = zmin = 1000000.0;
	xmax = ymax = zmax = -1000000.0;
	for(i=0; i<nv/3; i++) {
		if(vertices[3*i] < xmin)
			xmin = vertices[3*i];
		if(vertices[3*i] > xmax)
			xmax = vertices[3*i];
		if(vertices[3*i+1] < ymin)
			ymin = vertices[3*i+1];
		if(vertices[3*i+1] > ymax)
			ymax = vertices[3*i+1];
		if(vertices[3*i+2] < zmin)
			zmin = vertices[3*i+2];
		if(vertices[3*i+2] > zmax)
			zmax = vertices[3*i+2];
	}
	/* compute center and print range */
	cx = (xmin+xmax)/2.0f;
	cy = (ymin+ymax)/2.0f;
	cz = (zmin+zmax)/2.0f;
	printf("X range: %f %f\n",xmin,xmax);
	printf("Y range: %f %f\n",ymin,ymax);
	printf("Z range: %f %f\n",zmin,zmax);
	printf("center: %f %f %f\n",cx, cy,cz);

	/*  Retrieve the vertex normals */

	nn = shapes[0].mesh.normals.size();
	normals = new GLfloat[nn];
	for(i=0; i<nn; i++) {
		normals[i] = shapes[0].mesh.normals[i];
	}

	nt = shapes[0].mesh.texcoords.size();
	texcoords = new GLfloat[nt];
	for(i=0; i<nt; i++) {
		texcoords[i] = shapes[0].mesh.texcoords[i];
	}

	/*  Retrieve the triangle indices */

	ni = shapes[0].mesh.indices.size();
	triangles = ni/3;
	indices = new GLuint[ni];
	for(i=0; i<ni; i++) {
		indices[i] = shapes[0].mesh.indices[i];
	}

	/*
	 *  load the vertex coordinate data
	 */
	glGenBuffers(1, &vbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
	glBufferData(GL_ARRAY_BUFFER, (nv+nn+nt)*sizeof(GLfloat), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, nv*sizeof(GLfloat), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, nv*sizeof(GLfloat), nn*sizeof(GLfloat), normals);
	glBufferSubData(GL_ARRAY_BUFFER, (nv+nn)*sizeof(GLfloat), nt*sizeof(GLfloat), texcoords);
	
	/*
	 *  load the vertex indexes
	 */
	glGenBuffers(1, &ibuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ni*sizeof(GLuint), indices, GL_STATIC_DRAW);

	/*
	 *  compile and build the shader program
	 */
	
	vs = buildShader(GL_VERTEX_SHADER, "example8.vs");
	fs = buildShader(GL_FRAGMENT_SHADER, "example8.fs");
	program = buildProgram(vs,fs,0);

	/*
	 *  link the vertex coordinates to the vPosition
	 *  variable in the vertex program.  Do the same
	 *  for the normal vectors.
	 */
	glUseProgram(program);
	vPosition = glGetAttribLocation(program,"vPosition");
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);
	vNormal = glGetAttribLocation(program, "vNormal");
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, (void*) (nv*sizeof(vertices)));
	glEnableVertexAttribArray(vNormal);
	vTexcoord = glGetAttribLocation(program, "vTexcoord");
	glVertexAttribPointer(vTexcoord, 2, GL_FLOAT, GL_FALSE, 0, (void*) ((nv+nn)*sizeof(vertices)));
	glEnableVertexAttribArray(vTexcoord);

		/*
	 *  Create the texture.
	 */
    makeCheckImage();

	texture = loadImage("glasswork.jpg");
    
    glGenTextures(1, &texName);
    glBindTexture(GL_TEXTURE_2D, texName);
    
	/*
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, checkImageWidth, 
	checkImageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 
	&checkImage[0][0][0]);
	*/
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture->width, texture->height,
		0, GL_RGB,GL_UNSIGNED_BYTE,  texture->data);
	glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

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

	float ratio = 1.0f * w / h;

    glViewport(0, 0, w, h);

	projection = glm::perspective(45.0f, ratio, 1.0f, 800.0f);
	
}

/*
 *  This procedure is called each time the screen needs
 *  to be redisplayed
 */
void displayFunc() {
	glm::mat4 view;
	int modelViewLoc;
	int projectionLoc;
	int normalLoc;
	int eyeLoc;

	view = glm::lookAt(glm::vec3(eyex, eyey, eyez),
					glm::vec3(cx,cy,cz),
					glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat3 normal = glm::transpose(glm::inverse(glm::mat3(view)));

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(program);
	modelViewLoc = glGetUniformLocation(program,"modelView");
	glUniformMatrix4fv(modelViewLoc, 1, 0, glm::value_ptr(view));
	projectionLoc = glGetUniformLocation(program,"projection");
	glUniformMatrix4fv(projectionLoc, 1, 0, glm::value_ptr(projection));
	normalLoc = glGetUniformLocation(program,"normalMat");
	glUniformMatrix3fv(normalLoc, 1, 0, glm::value_ptr(normal));
	eyeLoc = glGetUniformLocation(program, "eye");

	glUniform3f(eyeLoc, eyex, eyey, eyez);

	glBindTexture(GL_TEXTURE_2D, texName);

	glBindVertexArray(objVAO);
	glDrawElements(GL_TRIANGLES, 3*triangles, GL_UNSIGNED_INT, NULL);

	glutSwapBuffers();

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
	}

	eyex = r*sin(theta)*cos(phi);
	eyey = r*sin(theta)*sin(phi);
	eyez = r*cos(theta);

	glutPostRedisplay();

}

int main(int argc, char **argv) {

	/*
	 *  initialize glut, set some parameters for
	 *  the application and create the window
	 */
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(500,500);
	window = glutCreateWindow("Example Eight");

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

	eyex = 0.0;
	eyey = 0.0;
	eyez = 500.0;

	theta = 0.5;
	phi = 1.5;
	r = 500.0;

	init();

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.3,0.3,0.3,1.0);

	glutMainLoop();

}