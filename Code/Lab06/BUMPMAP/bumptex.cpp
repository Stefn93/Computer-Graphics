#include <math.h>
#include "../common/common.h"

#define N 256
#define a 0.1
#define b 50.0

GLfloat normals[N][N][3];
GLfloat lightPos[4] = {0.0, 10.0, 0.0, 1.0};

GLuint program = 0;

GLuint texHandle;
GLuint texMapLocation;
GLint tangentParam;
const GLfloat tangent[3] = {1.0, 0.0, 0.0};


void initNormals() {
	int i, j, k;

	//spostare nella init
	for (i = 0; i<N; i++)
	{
		for (j = 0; j<N; j++)
		{
			normals[i][j][0] = 0.5;
			normals[i][j][1] = 0.5;
			normals[i][j][2] = 1.0;
		}
	}

	i = N / 2;
	for (j = 0; j<N; j++)
	{
		for (i = N / 4; i<N / 2; i += 8) {
			normals[i][j][0] = 0.5;
			normals[i][j][1] = 0.5;
			normals[i][j][2] = 0.0;
		}
		for (i = N / 2; i<3 * N / 4; i += 8) {
			normals[i][j][0] = 0.5;
			normals[i][j][1] = 0.0;
			normals[i][j][2] = 0.0;
		}
	}

	//   float data[N+1][N+1];
	//   for(i=0;i<N+1;i++)
	//       for(j=0;j<N+1;j++)
	//           data[i][j]=0.0;

	//   for(i=N/4; i< 3*N/4; i++)
	//       for(j=N/4;j<3*N/4;j++)
	//           data[i][j] = 100.5;

	////normal map
	//   for(i=0;i<N;i++)
	//   {
	//       for(j=0;j<N;j++)
	//       {
	//		//calcolo le derivate parziali per ogni punto per x e y --> ho creato la normal map (questa normal map è tra -1 e 1, vorremmo avere una texture map, la convertiamo)
	//           normals[i][j][0] = data[i][j]-data[i+1][j];
	//           normals[i][j][1] = data[i][j]-data[i][j+1];
	//           normals[i][j][2] = 1.0;
	//       }

	float d;

	//conversione a texture map
	for (i = 0; i<N; i++)
	{
		for (j = 0; j<N; j++)
		{
			d = 0.0;
			for (k = 0; k<3; k++)
				d += normals[i][j][k] * normals[i][j][k];
			d = sqrt(d);
			for (k = 0; k<3; k++)
				normals[i][j][k] = 0.5*normals[i][j][k] / d + 0.5;	//RGB

			//printf("D[%d][%d] = %.2f\n", i, j, data[i][j]);
			//printf("N[%d][%d] = %.2f %.2f %.2f\n", i, j, normals[i][j][0], normals[i][j][1], normals[i][j][2]);
		}
	}
}

//partiamo da una height map, costruiamo una normal map (mappa delle normali).
//Bisogna cambiare l'height map
//la luce che ruota fa notare il bump mapping
static void init()
{
	initNormals();
    const float meshColor[]     = {0.7f, 0.7f, 0.7f, 1.0f};
    const float meshSpecular[]  = {0.8f, 0.8f, 0.8f, 1.0f};
    const float meshShininess[] = {80.0f};

    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, meshColor);
    glMaterialfv(GL_FRONT, GL_SPECULAR, meshSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, meshShininess);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);


    glGenTextures(1, &texHandle);
    glBindTexture(GL_TEXTURE_2D, texHandle);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, N, N, 0, GL_RGB, GL_FLOAT, normals);
    glEnable(GL_TEXTURE_2D);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-0.75,0.75,-0.75,0.75,-5.5,5.5);

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);

    glewInit();
    program = initShader("v.glsl", "f.glsl");

    tangentParam = glGetAttribLocation(program, "objTangent");
    texMapLocation = glGetUniformLocation(program, "texMap");
}

void mesh()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(2.0, 2.0, 2.0, 0.5, 0.0, 0.5, 0.0, 1.0, 0.0);

    glNormal3f(0.0,1.0,0.0);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0);
        glVertex3f(0.0, 0.0, 0.0);
        glTexCoord2f(1.0, 0.0);
        glVertex3f(1.0, 0.0, 0.0);
        glTexCoord2f(1.0, 1.0);
        glVertex3f(1.0,0.0,1.0);
        glTexCoord2f(0.0, 1.0);
        glVertex3f(0.0,0.0,1.0);
    glEnd();
}

static void draw(void)
{
    glVertexAttrib3fv(tangentParam, tangent);
    glUniform1i(texMapLocation, 0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mesh();
    /* glutSolidTeapot(0.4); */

    glutSwapBuffers();
}

void idle()
{
    int t = glutGet(GLUT_ELAPSED_TIME);
    lightPos[0] = 5.5*sin(0.001*t);
    lightPos[2] = 5.5*cos(0.001*t);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(512, 512);
    glutCreateWindow("Simple GLSL example");
    glutDisplayFunc(draw);
    glutKeyboardFunc(commonKeyboard);
    glutIdleFunc(idle);

    init();

    glutMainLoop();
    return 0;
}
