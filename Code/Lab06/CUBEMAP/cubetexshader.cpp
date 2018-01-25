#include "../common/common.h"
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <GL/freeglut.h>
extern "C"
{
	#include "RgbImage.h"
}

GLuint program;
GLuint tex;
GLuint textureName[6];
static RgbImage textureData[6];

GLenum mode = GL_FALSE;

const char* filenameArray[6] = {
	"textures/posx2.bmp",
	"textures/negx2.bmp",
	"textures/posy2.bmp",
	"textures/negy2.bmp",
	"textures/posz2.bmp",
	"textures/negz2.bmp"
};

void loadTextureFromFile(const char *filename, int i) {
	RgbImage theTexMap;

	if (!RgbImageInitFile(&theTexMap, filename)) {
		exit(1);
	}

	// Pixel alignment: each row is word aligned.  Word alignment is the default.
	// glPixelStorei(GL_UNPACK_ALIGNMENT, 4);		

	// Set the interpolation settings to best quality.
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, GetNumCols(&theTexMap), GetNumRows(&theTexMap),
		GL_RGB, GL_UNSIGNED_BYTE, ImageData(&theTexMap));

	textureData[i] = theTexMap;
}


void initTextures(const char* filenames[]) {

	glGenTextures(6, textureName);
	for (int i = 0; i < 6; i++) {
		glBindTexture(GL_TEXTURE_2D, textureName[i]);		// Texture #i is active now
		loadTextureFromFile(filenames[i], i);					// Load texture #i
	}
}

void init()
{
    GLubyte red[3] = {255, 0, 0};
    GLubyte green[3] = {0, 255, 0};
    GLubyte blue[3] = {0, 0, 255};
    GLubyte cyan[3] = {0, 255, 255};
    GLubyte magenta[3] = {255, 0, 255};
    GLubyte yellow[3] = {255, 255, 0};


    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    glEnable(GL_TEXTURE_GEN_R);
    glEnable(GL_TEXTURE_CUBE_MAP);


	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
	glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
	
	if (mode == 0) {
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_CUBE_MAP, tex);

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, 3, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, red);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, 3, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, green);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, 3, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, blue);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, 3, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, cyan);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, 3, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, magenta);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, 3, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, yellow);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
	else {
		initTextures(filenameArray);

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, 3, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData[0].ImagePtr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, 3, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData[1].ImagePtr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, 3, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData[2].ImagePtr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, 3, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData[3].ImagePtr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, 3, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData[4].ImagePtr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, 3, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData[5].ImagePtr);
		
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);
		
	}

	glClearColor(1.0, 1.0, 1.0, 1.0);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-2.0, 2.0, -2.0, 2.0, -10.0, 10.0);
	glMatrixMode(GL_MODELVIEW);

    glewInit();
    program = initShader("v.glsl", "f.glsl");

    GLuint texMapLocation;
    texMapLocation = glGetUniformLocation(program, "texMap");
    glUniform1i(texMapLocation, 0);
}

void draw(void)
{
    double t = (double)glutGet(GLUT_ELAPSED_TIME);
    double k = 0.05 * 360.0/1000.0;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (mode == 0)
		glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
	else
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureName[1]);
    glPushMatrix();
        //glLoadIdentity();
        glTranslatef(0.0f, 0.0f, -5.0f);
        glRotatef(k*t, 1.0, 0.0, 0.0);
        glRotatef(k*t, 0.0, 1.0, 0.0);
        glutSolidTeapot(1.0);
    glPopMatrix();

    glutSwapBuffers();
    glutPostRedisplay();
}


void keyboard(unsigned char key, int x, int y) {
	switch (key) 
	{
		case 'a':
			mode = !mode;
			init();
		break;
		case 27:
			exit(1);
	}
}


int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutCreateWindow("colorcube environment");

    glutKeyboardFunc(keyboard);
    glutDisplayFunc(draw);

    init();

    glutMainLoop();
    return 0;
}
