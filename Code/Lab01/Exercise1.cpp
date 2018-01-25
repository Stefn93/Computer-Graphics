/*
* Usage:
*   Left click to place a control point.
*		Maximum number of control points allowed is currently set at 64.
*	 Press "f" to remove the first control point
*	 Press "l" to remove the last control point.
*	 Press escape to exit.
*/

#include <stdlib.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <math.h>
#include <iostream>

#define MAX_CV 64

/* prototypes */
void removeFirstPoint();
void removeLastPoint();
void removeAllPoints();
void initRendering();
void addNewPoint(float x, float y);
void movePoint(int x, int y);
float adaptX(int x);
float adaptY(int y);
float lerp(float x, float y, float t);
void de_casteljau(GLfloat c);
void adaptiveDecasteljau(float CV[MAX_CV][3], GLfloat t);
/*---------------*/

/* global variables */
float CV[MAX_CV][3];					//vettore dei dots
int numCV = 0;							//Numero iniziale di dots
const float parameter = 0.5f;			//Parametro di Casteljau
const float threshold = 0.001f;			//Soglia per il flat test

										//Window size in pixels
int WindowHeight;
int WindowWidth;

// Program mode
int mode = 0;
/*---------------*/

/* Custom functions */
//adapt x coordinates to window dimensions
float adaptX(int x)
{
	return ((float)x) / ((float)(WindowWidth - 1));
}

//adapt y coordinates to window height and flip the view
float adaptY(int y)
{
	return (1.0f - ((float)y) / ((float)(WindowHeight - 1)));
}

//De Casteljau function in substitution to the standard Bézier function
void de_casteljau(GLfloat t)
{
	float CJ[MAX_CV][3];
	for (int i = 0; i < MAX_CV; i++)
		for (int j = 0; j < 3; j++)
			CJ[i][j] = CV[i][j];

	for (int i = 1; i < numCV; i++)
	{
		for (int j = 0; j < numCV - i; j++)
		{
			CJ[j][0] = lerp(CJ[j][0], CJ[j + 1][0], t);		//lerp per x
			CJ[j][1] = lerp(CJ[j][1], CJ[j + 1][1], t);		//lerp per y
		}
	}
	glVertex2f(CJ[0][0], CJ[0][1]);
}

//Linear interpolation
float lerp(float x0, float x1, float t) {
	return (1 - t) * x0 + t * x1;
}

//flat test to verify how flat is the considered curve 
bool flatTest(float dist) {
	if (dist < threshold)
		return true;
	return false;
}

/* Calculate point-line distance */
float getDistancePL(float outx, float outy, float x2, float y2, float x3, float y3) {
	float a = y2 - y3;
	float b = x3 - x2;
	float c = (x2 - x3) * y2 + (y3 - y2) * x2;

	return abs((a * outx + b * outy + c) / sqrt(a*a + b*b));
}

/* Curve subdivision with de casteljau algorithm */
void adaptiveDecasteljau(float CV[MAX_CV][3], GLfloat t) {

	/* Dichiarazioni */
	float CJ[MAX_CV][2];
	float dist[MAX_CV];			//vettore delle distanze
	int k = 0;
	int z = 0;
	bool recursion = false;
	/*---------------*/

	/*Inizializzo la copia della matrice*/
	for (int i = 0; i < numCV; i++)
		for (int j = 0; j < 2; j++)
			CJ[i][j] = CV[i][j];
	/*----------------------------------*/

	/* Ciclo sulle distanze ed eseguo il flatTest */
	for (int f = 1; f < numCV - 1; f++) {
		dist[f] = getDistancePL(CJ[f][0], CJ[f][1], CJ[0][0], CJ[0][1], CJ[numCV - 1][0], CJ[numCV - 1][1]);

		if (!flatTest(dist[f]))
			recursion = true;			//se il flat test non è superato, si va in ricorsione applicando la suddivisione adattiva
	}
	/*-------------------------------------------*/

	if (recursion) {
		float left[MAX_CV][3];			//matrice ricorsione a sinistra
		float right[MAX_CV][3];			//matrice ricorsione a destra

		left[k][0] = CJ[0][0];
		left[k++][1] = CJ[0][1];
		right[z][0] = CJ[numCV - 1][0];
		right[z++][1] = CJ[numCV - 1][1];
		for (int i = 1; i < numCV; i++)
		{
			for (int j = 0; j < numCV - i; j++)
			{
				CJ[j][0] = lerp(CJ[j][0], CJ[j + 1][0], t);
				CJ[j][1] = lerp(CJ[j][1], CJ[j + 1][1], t);
			}
			left[k][0] = CJ[0][0];
			left[k++][1] = CJ[0][1];
			right[z][0] = CJ[numCV - i - 1][0];
			right[z++][1] = CJ[numCV - i - 1][1];
		}

		adaptiveDecasteljau(left, (float)(0.5 * t));
		adaptiveDecasteljau(right, (float)(t + ((1 - t) * 0.5)));
	}
	else {
		glColor3f(0.0f, 0.0f, 1.0f);
		glBegin(GL_LINE_STRIP);
		glVertex2f(CJ[0][0], CJ[0][1]);
		glVertex2f(CJ[numCV - 1][0], CJ[numCV - 1][1]);
		glEnd();
	}
}


//Callback function of glutMotionFunc
void movePoint(int x, int y) {

	for (int i = 0; i < numCV; i++) {
		//Euclidean distance between the click coordinates and every plotted point
		float dist = sqrt(pow((CV[i][0] - adaptX(x)), 2) + pow((CV[i][1] - adaptY(y)), 2));
		if (dist < 0.1f) {
			CV[i][0] = adaptX(x);
			CV[i][1] = adaptY(y);
			glutPostRedisplay();
		}
	}
}

/* Initial functions */

// Add a new point to the end of the list.  
// Remove the first point in the list if too many points.
void addNewPoint(float x, float y)
{
	if (numCV >= MAX_CV)
		removeFirstPoint();

	CV[numCV][0] = x;
	CV[numCV][1] = y;
	CV[numCV][2] = 0.0f;
	numCV++;
}

//Remove the oldest point from the list.
void removeFirstPoint()
{
	if (numCV > 0)
	{
		// Remove the first point, slide the rest down
		numCV--;
		for (int i = 0; i < numCV; i++)
		{
			CV[i][0] = CV[i + 1][0];
			CV[i][1] = CV[i + 1][1];
		}
	}
}

// Add a new point to the end of the list.  
// Remove the first point in the list if too many points.
void removeLastPoint()
{
	if (numCV > 0)
		numCV--;
}
/*---------------*/

//Remove all inserted points
void removeAllPoints()
{
	numCV = 0;
}

//Keyboard events
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'f':
		removeFirstPoint();
		glutPostRedisplay();
		break;
	case 'l':
		removeLastPoint();
		glutPostRedisplay();
		break;
	case 'c':
		mode = 1;
		removeAllPoints();
		glutPostRedisplay();
		break;
	case 'a':
		mode = 2;
		removeAllPoints();
		glutPostRedisplay();
		break;
	case 'o':
		mode = 0;
		removeAllPoints();
		initRendering();
		glutPostRedisplay();
		break;
	case 27:	 // Escape key
		exit(0);
		break;
	}
}

//Mouse events
void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		glutMotionFunc(NULL);
		addNewPoint(adaptX(x), adaptY(y));
		glutPostRedisplay();
	}
	else if (button == GLUT_RIGHT_BUTTON)
	{
		glutMotionFunc(movePoint);
	}
}

//Display function
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	// Draw the line segments
	if (numCV > 1)
	{
		//Stipple lines that directly connect dots
		glColor3f(0.5f, 0.5f, 0.0f);	   //greenish stripped lines
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(0.5, 0x00FF);
		glBegin(GL_LINE_STRIP);		   //connette tutti i punti interpolati
		for (int i = 0; i < numCV; i++)
			glVertex2f(CV[i][0], CV[i][1]);
		glEnd();
		glDisable(GL_LINE_STIPPLE);

		/*Adaptive decasteljau*/
		if (mode == 2)
			adaptiveDecasteljau(CV, parameter);
		/*--------------------*/

		if (mode == 0)
			glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, numCV, &CV[0][0]);

		if (mode == 0 || mode == 1) {
			/* Classical decasteljau */
			glColor3f(0.0f, 0.0f, 1.0f);
			glBegin(GL_LINE_STRIP);
			int iterNum = 100;
			for (int i = 0; i < iterNum; i++)
			{
				GLfloat t = (GLfloat)i / (iterNum - 1);
				if (mode == 1)
					de_casteljau(t);
				else if (mode == 0)
					glEvalCoord1f(t);
			}
			glEnd();
			/*----------------------*/
		}
	}

	// Draw the interpolated points second.
	glColor3f(1.0f, 0.0f, 0.0f);	   // Draw points in red
	glBegin(GL_POINTS);
	for (int i = 0; i < numCV; i++)
		glVertex2f(CV[i][0], CV[i][1]);
	glEnd();

	glFlush();
}


void initRendering()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	// Make big points and wide lines.  (This may be commented out if desired.)
	glPointSize(6);
	glLineWidth(2);

	// The following commands should induce OpenGL to create round points and 
	//  antialias points and lines.  (This is implementation dependent unfortunately, and
	//  may slow down rendering considerably.)
	//  You may comment these out if you wish.
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);	// Make round points, not square points
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);	// Antialias the lines
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	if (mode == 0)
		glEnable(GL_MAP1_VERTEX_3);
}

void reshape(int w, int h)
{
	WindowHeight = (h > 1) ? h : 2;
	WindowWidth = (w > 1) ? w : 2;
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0f, 1.0f, 0.0f, 1.0f);	// Always view [0,1]x[0,1].
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);

	initRendering();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);

	glutMainLoop();
	return 0;		// This line is never reached
}


