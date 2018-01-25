/*  CG LAB2
Gestione interattiva di una scena 3D mediante controllo
da mouse e da tastiera. I modelli geometrici in scena 
sono primitive GLU e mesh poligonali in formato *.m

 * INPUT: file .m contenente la mesh a triangoli:
 *        Vertex  NUM  x y z 
 *                .......
 *        Normal  NUM  x y z 
 *                .......
 *        Face    NUM f1 f2 f3 
 *                .......
 * OUPUT: visualizzazione della mesh in una finestra OpenGL
 *
 *
 * NB: i vertici di ogni faccia sono in verso orario
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

#if defined(__APPLE__) || defined(MACOSX)
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif

#define MAX_V 10000 /* max number of vertices allowed in the mesh model */
#define M_PI 3.141592653589793
#define MAX_CV 4

int  	wireframe; /* controls the visualization of primitives via glPolygonMode */                
int     orpro;     /* controls the type of projection via gluPerspective and glOrtho */
int     cull;      /* toggles backface culling via glEnable( GL_CULL_FACE ) and glDisable( GL_CULL_FACE ); */
int     mater;     /* controls the material associated with the model via glMaterial */
int     shading;   /* controls the shading model via glShadeModel */
int		zoom;	   /* controls the zooming factor on the scene */
int		animation;

GLfloat fovy;	/* angolo del punto di vista */
GLfloat angle[3]; /* rotazione degli assi WCS*/

GLfloat camC[3]; /* centro del sistema */
GLfloat camE[3]; /* punto di vista */
GLfloat camU[3]; /* vettore su della camera */
GLfloat lightPos[4]; /* posizione della luce */

GLfloat brass_ambient[] = {0.33, 0.22, 0.03, 1.0}, brass_diffuse[] = {0.78, 0.57, 0.11, 1.0}, brass_specular[] = {0.99, 0.91, 0.81, 1.0}, brass_shininess[] = {27.8};
GLfloat red_plastic_ambient[] = {0.0, 0.0, 0.0}, red_plastic_diffuse[] = {0.5, 0.0, 0.0}, red_plastic_specular[] = {0.7, 0.6, 0.6}, red_plastic_shininess[] = {32.0};
GLfloat emerald_ambient[] = {0.0215, 0.1745, 0.0215}, emerald_diffuse[] = {0.07568, 0.61424, 0.07568}, emerald_specular[] = {0.633, 0.727811, 0.633}, emerald_shininess[] = {76.8};
GLfloat slate_ambient[] = {0.02, 0.02, 0.02}, slate_diffuse[] = {0.02, 0.01, 0.01}, slate_specular[] = {0.4, 0.4, 0.4}, slate_shininess[] = {.78125};

GLfloat aspect = 1.0; /* rapporto larghezza-altezza della viewport */

GLUquadricObj* myReusableQuadric = 0;

/* Trackball variables */
float tbAngle = 0.0;
float tbAxis[3];

int updateTB = 0;
int tbDragging = 0;
int allowTB = 0;
float tbV[3];
float tbW[3];

/* Orthogonal variables */
float orthoLeft = 999.0f; 
float orthoRight = -999.0f;
float orthoBottom = 999.0f;
float orthoTop = -999.0f;
float orthoNear = 999.0f;
float orthoFar = -999.0f;
void findMinMax(float a, float b, float c);
float maxDistance();

/* Bézier curve vector */
float CV[MAX_CV][2];
float CJ[MAX_CV][2];
void init_CV();
void de_casteljau(GLfloat t);
void callDC();
GLfloat stepA = 0.000f;

void print_sys_status();
void plotModel(char fileName[30], int listname);

/* Object Transform */
void rotateCS(unsigned char key);
void translateCS(unsigned char key);

int selectedObj = 0;
int objChange = 1;
int initObjs = 1;
float objTrasl[3];
float rotAxis;
float objStep;

float tempRot[3][3] = { { 0.0, 0.0, 0.0 },{ 0.0, 0.0, 0.0 },{ 0.0, 0.0, 0.0 } };

float trackballMatrix[16] = { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f };

float ocsTraslMatrix[3][16] = {{ 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f },
								{ 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f },
								{ 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f }};

float ocsMatrix[3][16] = {{ 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f },
						    { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f },
						    { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f }};

float wcsTraslMatrix[3][16] = {{ 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f },
								{ 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f },
								{ 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f }};

float wcsMatrix[3][16] = {{ 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f },
						   { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f },
						   { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f }};

float vcsTraslMatrix[3][16] = {{ 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f },
								{ 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f },
								{ 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f }};

float vcsRotMatrix[3][16] = { { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f },
							{ 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f },
							{ 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f } };


enum Modes
{
	MODE_INVALID,
	MODE_CHANGE_EYE_POS,
	MODE_CHANGE_REFERENCE_POS,
	MODE_CHANGE_UP_POS,
	MODE_CHANGE_LIGHT_POS,
	MODE_CHANGE_ZOOM,
	MODE_ROTATE_WORLD,

	MODE_CHANGE_CULLING,
	MODE_CHANGE_WIREFRAME,
	MODE_CHANGE_SHADING,
	MODE_CHANGE_PROJECTION,
	MODE_CHANGE_MATERIAL,
	MODE_ANIMATION,

	MODE_TRANSLATE_WCS,
	MODE_ROTATE_WCS,
	MODE_TRANSLATE_OCS,
	MODE_ROTATE_OCS,
	MODE_TRANSLATE_VCS,
	MODE_ROTATE_VCS,

	MODE_PRINT_SYSTEM_STATUS,
	MODE_RESET,
	MODE_QUIT
};

enum Modes mode = MODE_CHANGE_EYE_POS; /* global variable that stores the current mode */

int WindowWidth = 700;
int WindowHeight = 700;

/*Disegna gli assi sulla scena, i vertici nella funzione rappresentano direttamente le lettere disegnate*/
void drawAxis( float scale, int drawLetters )
{	
	glDisable( GL_LIGHTING );
	glPushMatrix();
	glScalef( scale, scale, scale );
	glBegin( GL_LINES );

	glColor4d( 1.0, 0.0, 0.0, 1.0 );
	if( drawLetters )
	{
		glVertex3f( .8f, 0.05f, 0.0 );  glVertex3f( 1.0, 0.25f, 0.0 ); /* Letter X */
		glVertex3f( 0.8f, .25f, 0.0 );  glVertex3f( 1.0, 0.05f, 0.0 );
	}
	glVertex3f( 0.0, 0.0, 0.0 );  glVertex3f( 1.0, 0.0, 0.0 ); /* X axis      */


	glColor4d( 0.0, 1.0, 0.0, 1.0 );
	if( drawLetters )
	{
		glVertex3f( 0.10f, 0.8f, 0.0 );   glVertex3f( 0.10f, 0.90f, 0.0 ); /* Letter Y */
		glVertex3f( 0.10f, 0.90f, 0.0 );  glVertex3f( 0.05, 1.0, 0.0 );
		glVertex3f( 0.10f, 0.90f, 0.0 );  glVertex3f( 0.15, 1.0, 0.0 );
	}
	glVertex3f( 0.0, 0.0, 0.0 );  glVertex3f( 0.0, 1.0, 0.0 ); /* Y axis      */


	glColor4d( 0.0, 0.0, 1.0, 1.0 );
	if( drawLetters )
	{
		glVertex3f( 0.05f, 0, 0.8f );  glVertex3f( 0.20f, 0, 0.8f ); /* Letter Z*/
		glVertex3f( 0.20f, 0, 0.8f );  glVertex3f( 0.05, 0, 1.0 );
		glVertex3f( 0.05f, 0, 1.0 );   glVertex3f( 0.20, 0, 1.0 );
	}
	glVertex3f( 0.0, 0.0, 0.0 );  glVertex3f( 0.0, 0.0, 1.0 ); /* Z axis    */

	glEnd();
	glPopMatrix();

	glEnable( GL_LIGHTING );
}

void computePointOnTrackball( int x, int y, float p[3] )
{
	float zTemp;
	//map to [-1;1]
	p[0] = (2.0f * x - WindowWidth)  / WindowWidth;
	p[1] = (WindowHeight - 2.0f * y) / WindowHeight;

	zTemp = 1.0f - (p[0]*p[0]) - (p[1]*p[1]);
	p[2] = (zTemp > 0.0f ) ? sqrt(zTemp) : 0.0;

	//printf( "p = (%.2f, %.2f, %.2f)\n", p[0], p[1], p[2] );
}

void mouse( int button, int state, int x, int y )
{
	if( button==GLUT_LEFT_BUTTON && state==GLUT_DOWN ) 
	{
		tbDragging = 1;
		computePointOnTrackball(x, y, tbV);
	}
	if(button==GLUT_LEFT_BUTTON && state==GLUT_UP) 
	{
		tbDragging = 0; 
		updateTB = 1;
		glutPostRedisplay();
	}
}

void motion( int x, int y )
{
	float dx, dy, dz;
	
	computePointOnTrackball(x, y, tbW);
	allowTB = 0;
	if (tbDragging)
	{
		dx = tbV[0] - tbW[0];
		dy = tbV[1] - tbW[1];
		dz = tbV[2] - tbW[2];
		if (dx || dy || dz)
		{
			tbAngle = sqrt(dx*dx + dy*dy + dz*dz) * (180.0 / M_PI);
			tbAxis[0] = tbW[1] * tbV[2] - tbW[2] * tbV[1];
			tbAxis[1] = tbW[2] * tbV[0] - tbW[0] * tbV[2];
			tbAxis[2] = tbW[0] * tbV[1] - tbW[1] * tbV[0];
			allowTB = 1;
		}
		//printf("tbAngle = %.2f tbAxis = (%.2f, %.2f, %.2f)\n", tbAngle, tbAxis[0], tbAxis[1], tbAxis[2]);
	}
	glutPostRedisplay();
}

void special( int key, int x, int y )
{
	
}

void keyboard (unsigned char key, int x, int y) 
{	
	float* pos = NULL;
	float step;

	/* Select Object */
	if (key == '1' && selectedObj == 1) {
		selectedObj = 0;
	}
	else if (key == '2' && selectedObj == 2) {
		selectedObj = 0;
	}
	else if (key == '3' && selectedObj == 3) {
		selectedObj = 0;
	}
	else if (key == '1') {
		selectedObj = 1;
	}
	else if (key == '2') {
		selectedObj = 2;
	}
	else if (key == '3') {
		selectedObj = 3;
	}
	/******************/

	/* standard modes */
	if ((mode == MODE_ROTATE_OCS || mode == MODE_ROTATE_WCS || mode == MODE_ROTATE_VCS) && selectedObj != 0) 
	{
		rotateCS(key);
	}
	else if ((mode == MODE_TRANSLATE_OCS || mode == MODE_TRANSLATE_WCS || mode == MODE_TRANSLATE_VCS) && selectedObj != 0)
	{
		translateCS(key);
	}
	else if( mode == MODE_CHANGE_EYE_POS )
	{
		pos = camE;
		step = 1.0;
	}
	else if( mode == MODE_CHANGE_REFERENCE_POS )
	{
		pos = camC;
		step = 1.0;
	}
	else if( mode == MODE_CHANGE_UP_POS )
	{
		pos = camU;
		step = 1.0;
	}
	else if( mode == MODE_CHANGE_LIGHT_POS )
	{
		pos = lightPos;
		step = 3.0;
	}
	else if( mode == MODE_ROTATE_WORLD )
	{
		pos = angle;
		step = 2.0;
	}
	/************************/

	/* Standard mode actions*/
	if( pos != NULL )
	{
		if( key == 'x' )
		   pos[0] += step;
		else if( key == 'X')
		   pos[0] -= step;
		else if( key == 'y')
		   pos[1] += step;
		else if( key == 'Y')
		   pos[1] -= step;
		else if( key == 'z')
		   pos[2] += step;
		else if( key == 'Z')
		   pos[2] -= step;
		glutPostRedisplay();
	}
	/************************/

	/* Zoom */
	if (zoom && key == 'f' && fovy > 1)
		fovy--;
	else if (zoom && key == 'F' && fovy < 40)
		fovy++;
	/********/

	/* Animation */
	if (stepA < 1.000f && key == 'a') {
		callDC();
		glutPostRedisplay();
		stepA += 0.005f;
	}
	else if (stepA = 1.000f) {
		callDC();
		glutPostRedisplay();
		stepA = 0.000f;
	}
	/*************/
	
	/* Rage quit */
	if(key ==  27) 
		exit(1);
}

void rotateCS(unsigned char key) {
	if (key == 'x') {
		rotAxis = 1;
		objStep = 2.0;
		objChange = 1;
	}
	else if (key == 'X') {
		rotAxis = 1;
		objStep = -2.0;
		objChange = 1;
	}
	else if (key == 'y') {
		rotAxis = 2;
		objStep = 2.0;
		objChange = 1;
	}
	else if (key == 'Y') {
		rotAxis = 2;
		objStep = -2.0;
		objChange = 1;
	}
	else if (key == 'z') {
		rotAxis = 3;
		objStep = 2.0;
		objChange = 1;
	}
	else if (key == 'Z') {
		rotAxis = 3;
		objStep = -2.0;
		objChange = 1;
	}
	glutPostRedisplay();
}

void translateCS(unsigned char key) {
	if (key == 'x') {
		objTrasl[0] = 0.2;
		objTrasl[1] = 0;
		objTrasl[2] = 0;
		objChange = 1;
	}
	else if (key == 'X') {
		objTrasl[0] = -0.2;
		objTrasl[1] = 0;
		objTrasl[2] = 0;
		objChange = 1;
	}
	else if (key == 'y') {
		objTrasl[0] = 0;
		objTrasl[1] = 0.2;
		objTrasl[2] = 0;
		objChange = 1;
	}
	else if (key == 'Y') {
		objTrasl[0] = 0;
		objTrasl[1] = -0.2;
		objTrasl[2] = 0;
		objChange = 1;
	}
	else if (key == 'z') {
		objTrasl[0] = 0;
		objTrasl[1] = 0;
		objTrasl[2] = 0.2;
		objChange = 1;
	}
	else if (key == 'Z') {
		objTrasl[0] = 0;
		objTrasl[1] = 0;
		objTrasl[2] = -0.2;
		objChange = 1;
	}
	glutPostRedisplay();
}

void drawGluSlantCylinderWithCaps( double height, double radiusBase, double radiusTop, int slices, int stacks )
{
	// First draw the cylinder
	gluCylinder( myReusableQuadric, radiusBase, radiusTop, height, slices, stacks );

	// Draw the top disk cap
	glPushMatrix();
	glTranslated(0.0, 0.0, height);
	gluDisk( myReusableQuadric, 0.0, radiusTop, slices, stacks );
	glPopMatrix();

	// Draw the bottom disk cap
	glPushMatrix();
	glRotated(180.0, 1.0, 0.0, 0.0);
	gluDisk( myReusableQuadric, 0.0, radiusBase, slices, stacks );
	glPopMatrix();
}

void trackBallRotation() {
	// Trackball rotation.
	glMultMatrixf(trackballMatrix);
	if (allowTB)
		glRotatef(tbAngle, tbAxis[0], tbAxis[1], tbAxis[2]);

	if (updateTB && allowTB) {
		glPushMatrix();
		glLoadMatrixf(trackballMatrix);
		glRotatef(tbAngle, tbAxis[0], tbAxis[1], tbAxis[2]);
		glGetFloatv(GL_MODELVIEW_MATRIX, trackballMatrix);
		glPopMatrix();
		updateTB = 0;
		allowTB = 0;
	}

	glLineWidth(2);
	drawAxis(2.0, 1);
	glLineWidth(1);

	glRotatef(angle[0], 1.0, 0.0, 0.0);
	glRotatef(angle[1], 0.0, 1.0, 0.0);
	glRotatef(angle[2], 0.0, 0.0, 1.0);
}

void display()
{
	if (mater==0) //ottone
	{	
		glLightfv(GL_LIGHT0, GL_AMBIENT, brass_ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, brass_diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, brass_specular);
		glMaterialfv(GL_FRONT, GL_SHININESS, brass_shininess);
	}
	if (mater==1) //plastica rossa
	{
		glLightfv(GL_LIGHT0, GL_AMBIENT, red_plastic_ambient);
   		glMaterialfv(GL_FRONT, GL_DIFFUSE, red_plastic_diffuse);
   		glMaterialfv(GL_FRONT, GL_SPECULAR, red_plastic_specular);
   		glMaterialfv(GL_FRONT, GL_SHININESS, red_plastic_shininess);
	}
   	if (mater==2) //smeraldo
   	{	
		glLightfv(GL_LIGHT0, GL_AMBIENT, emerald_ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, emerald_diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, emerald_specular);
		glMaterialfv(GL_FRONT, GL_SHININESS, emerald_shininess);
	}
	if (mater==3) //slate
	{	
		glLightfv(GL_LIGHT0, GL_AMBIENT, slate_ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, slate_diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, slate_specular);
		glMaterialfv(GL_FRONT, GL_SHININESS, slate_shininess);
	}

	glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );

	if (cull)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);

	if (wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if (shading)
		glShadeModel(GL_SMOOTH);
	else
		glShadeModel(GL_FLAT);

	glLightfv( GL_LIGHT0, GL_POSITION, lightPos);
	glEnable( GL_LIGHTING );
	glEnable( GL_LIGHT0 );

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (orpro)
		gluPerspective(fovy, aspect, 1, 100);
	else {
		glOrtho(orthoLeft - 2, orthoRight + 2, orthoBottom - 2, orthoTop + 2, -maxDistance(), maxDistance());
		//printf("l: %f,r: %f,b: %f,t: %f,n: %f,f: %f, d: %f\n", orthoLeft, orthoRight, orthoBottom, orthoTop, orthoNear, orthoFar, maxDistance());
	}

	/*********************************/
	/* Process Model Transformations */
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (orpro)
		drawAxis( 1.0, 0 );
	
	if (initObjs == 1) {
		glPushMatrix();
		glLoadMatrixf(ocsMatrix[1]);
		glTranslatef(2.0f, 0.0f, 0.0f);
		glGetFloatv(GL_MODELVIEW_MATRIX, ocsMatrix[1]);
		glPopMatrix();

		glPushMatrix();
		glLoadMatrixf(ocsMatrix[2]);
		glTranslatef(0.0f, 0.0f, 2.0f);
		glGetFloatv(GL_MODELVIEW_MATRIX, ocsMatrix[2]);
		glPopMatrix();

		initObjs = 0;
	}
	else if (mode == MODE_ROTATE_WCS && objChange) {
		/* WORLD ROTATION */
		glPushMatrix();
		if (rotAxis == 1) {
			glLoadMatrixf(wcsMatrix[selectedObj - 1]);
			glRotatef(objStep, 1.0f, 0.0f, 0.0f);
			glGetFloatv(GL_MODELVIEW_MATRIX, wcsMatrix[selectedObj - 1]);
		}
		else if (rotAxis == 2)
		{
			glLoadMatrixf(wcsMatrix[selectedObj - 1]);
			glRotatef(objStep, 0.0f, 1.0f, 0.0f);
			glGetFloatv(GL_MODELVIEW_MATRIX, wcsMatrix[selectedObj - 1]);
		}
		else if (rotAxis == 3)
		{
			glLoadMatrixf(wcsMatrix[selectedObj - 1]);
			glRotatef(objStep, 0.0f, 0.0f, 1.0f);
			glGetFloatv(GL_MODELVIEW_MATRIX, wcsMatrix[selectedObj - 1]);
		}
		glPopMatrix();
		/*******************/
		objChange = 0;
	}
	else if (mode == MODE_TRANSLATE_WCS && objChange) {

		/* WORLD TRANSLATION */
		glPushMatrix();
		glLoadMatrixf(wcsTraslMatrix[selectedObj - 1]);
		glTranslatef(objTrasl[0], objTrasl[1], objTrasl[2]);
		glGetFloatv(GL_MODELVIEW_MATRIX, wcsTraslMatrix[selectedObj - 1]);
		glPopMatrix();
		/*******************/

		objChange = 0;
	}
	else if (mode == MODE_ROTATE_OCS && objChange) {

		/* OBJECT ROTATION */
		glPushMatrix();
		if (rotAxis == 1) {
			glLoadMatrixf(ocsMatrix[selectedObj - 1]);
			glRotatef(objStep, 1.0f, 0.0f, 0.0f);
			glGetFloatv(GL_MODELVIEW_MATRIX, ocsMatrix[selectedObj - 1]);
		}
		else if (rotAxis == 2)
		{
			glLoadMatrixf(ocsMatrix[selectedObj - 1]);
			glRotatef(objStep, 0.0f, 1.0f, 0.0f);
			glGetFloatv(GL_MODELVIEW_MATRIX, ocsMatrix[selectedObj - 1]);
		}
		else if (rotAxis == 3)
		{
			glLoadMatrixf(ocsMatrix[selectedObj - 1]);
			glRotatef(objStep, 0.0f, 0.0f, 1.0f);
			glGetFloatv(GL_MODELVIEW_MATRIX, ocsMatrix[selectedObj - 1]);
		}
		glPopMatrix();
		/*******************/
		objChange = 0;
	}
	else if (mode == MODE_TRANSLATE_OCS && objChange) {

		/* OBJECT TRANSLATION */
		glPushMatrix();
			glLoadMatrixf(ocsTraslMatrix[selectedObj - 1]);
			glTranslatef(objTrasl[0], objTrasl[1], objTrasl[2]);
			glGetFloatv(GL_MODELVIEW_MATRIX, ocsTraslMatrix[selectedObj - 1]);
		glPopMatrix();
		/*******************/

		objChange = 0;
	}
	else if (mode == MODE_TRANSLATE_VCS && objChange) {

		/* VIEW TRANSLATION */
		glPushMatrix();
		glLoadMatrixf(vcsTraslMatrix[selectedObj - 1]);
		glTranslatef(objTrasl[0], objTrasl[1], objTrasl[2]);
		glGetFloatv(GL_MODELVIEW_MATRIX, vcsTraslMatrix[selectedObj - 1]);
		glPopMatrix();
		/*******************/

		objChange = 0;
	}
	else if (mode == MODE_ROTATE_VCS && objChange) {

		/* VIEW ROTATION */
		glPushMatrix();
		if (rotAxis == 1) {
			glLoadMatrixf(vcsRotMatrix[selectedObj - 1]);
			glRotatef(objStep, 1.0f, 0.0f, 0.0f);
			glGetFloatv(GL_MODELVIEW_MATRIX, vcsRotMatrix[selectedObj - 1]);
		}
		else if (rotAxis == 2)
		{
			glLoadMatrixf(vcsRotMatrix[selectedObj - 1]);
			glRotatef(objStep, 0.0f, 1.0f, 0.0f);
			glGetFloatv(GL_MODELVIEW_MATRIX, vcsRotMatrix[selectedObj - 1]);
		}
		else if (rotAxis == 3)
		{
			glLoadMatrixf(vcsRotMatrix[selectedObj - 1]);
			glRotatef(objStep, 0.0f, 0.0f, 1.0f);
			glGetFloatv(GL_MODELVIEW_MATRIX, vcsRotMatrix[selectedObj - 1]);
		}
		glPopMatrix();
		/*******************/

		objChange = 0;
	}

	/* Manipolazione dello stack delle matrici di trasformazione */
	for (int i = 0; i < 3; i++) {
		glPushMatrix();
		glMultMatrixf(vcsTraslMatrix[i]);
		glMultMatrixf(vcsRotMatrix[i]);
		gluLookAt(camE[0], camE[1], camE[2], camC[0], camC[1], camC[2], camU[0], camU[1], camU[2]);
		trackBallRotation();
		glMultMatrixf(wcsMatrix[i]);
		glMultMatrixf(wcsTraslMatrix[i]);
		drawAxis(1.0, 0);
		glMultMatrixf(ocsMatrix[i]);
		glMultMatrixf(ocsTraslMatrix[i]);
		glCallList(i + 1);
		glPopMatrix();
	}
	/*********************************/

	//Draw some quadrics
	//gluSphere(myReusableQuadric, 1.0, 12, 12);

	//glTranslated(0.0, 0.0, 1.5);
	//gluCylinder(myReusableQuadric, 0.5, 0.2, 0.5, 12, 12); 

	//gluDisk(myReusableQuadric, 0.5, 1.0, 10, 10);
	//gluPartialDisk( myReusableQuadric, 0.5, 1.0, 10, 10, 0.0, 45.0);
	//drawGluSlantCylinderWithCaps( 2.0, 1.0, 0.4, 8, 8 ); 

	//  glutWireIcosahedron();
	//	glutWireDodecahedron();
	//  glutWireTeapot(1.0); 
	//	glutWireTorus(0.5, 1.0, 10, 10);
	//	glutWireCone(1.0, 1.0, 10, 10);
	glutSwapBuffers();
}


void reset()
{
	angle[0] = 0.0;
	angle[1] = 0.0;
	angle[2] = 0.0;

	camE[0] = 8.8;
	camE[1] = 4.9;
	camE[2] = 9.0;

	camC[0] = 0.0;
	camC[1] = 0.0;
	camC[2] = 0.0;

	camU[0] = 0.0;
	camU[1] = 1.0;
	camU[2] = 0.0;

	lightPos[0] =  5.0;
	lightPos[1] =  5.0;
	lightPos[2] =  5.0;
	lightPos[3] =  1.0;

	fovy = 20;
	wireframe = 1;
	cull = 0;
	mater = 1;
	orpro = 1;
	shading = 0;
	zoom = 1;

	glutPositionWindow(10, 10);
	glutReshapeWindow(700, 700);
	glutPostRedisplay();
}

void plotModel(char fileName[30], int listname) {
	int 	faces[MAX_V][3];    /* faces */
	float 	vertices[MAX_V][3]; /* vertices */
	float 	vnormals[MAX_V][3]; /* vertex normals */
	float   fnormals[MAX_V][3]; /* face normals */
	int	vertOccurrences[MAX_V];

	int nrighe;
	int ids[3];
	FILE* idf;
	char s[10];
	float a, b, c;
	int nface, nvert, index;
	int* face;
	float* vert;

	for (int i = 0; i < MAX_V; i++) {
		vertOccurrences[i] = 0;
		for (int j = 0; j < 3; j++) {
			fnormals[i][j] = 0.0f;
			vnormals[i][j] = 0.0f;
		}
	}

	//apertura del file *.m
	printf("Apertura del file...\n");
#ifdef WIN32
	if ((idf = fopen(fileName, "r")) == NULL)
#else
	if ((idf = fopen("data/pig.m", "r")) == NULL)
#endif
	{
		perror("file non trovato\n");
		exit(1);
	}

	nface = 0, nvert = 0;

	while (!feof(idf))
	{
		fscanf(idf, "%s %d %f %f %f", s, &index, &a, &b, &c);
		switch (s[0])
		{
		case 'V':
			vert = vertices[nvert++];

			vert[0] = a;
			vert[1] = b;
			vert[2] = c;

			findMinMax(a, b, c);
			break;
		case 'N':
			printf(" Vertex normals not used! \n");
			break;
		case 'F':
			face = faces[nface];

			face[0] = (int) a - 1;
			face[1] = (int) b - 1;
			face[2] = (int) c - 1; 

			/* calcolo normali alle facce */
			float l1[3], l2[3];
			v3dSub(vertices[face[1]], vertices[face[0]], l1);
			v3dSub(vertices[face[2]], vertices[face[0]], l2);
			v3dCross(l1, l2, fnormals[nface]);

			/* Conto le occorrenze di ogni vertice nelle facce del modello */
			vertOccurrences[face[0]]++;
			vertOccurrences[face[1]]++;
			vertOccurrences[face[2]]++;

			nface++;
			break;
		case 'E':
			printf(" Edge not used! \n");
			break;
		default:
			printf(" ERROR: Reading file \n");
			break;
		}
	}

	nrighe = nvert + nface;
	printf("Chiusura del file...Vertices %d Faces %d -> %d\n", nvert, nface - 1, nrighe);
	fclose(idf);

	/* calcolo le normali ai vertici */
	for (int i = 0; i < nface; i++)
	{
		ids[2] = faces[i][0];
		ids[1] = faces[i][1];
		ids[0] = faces[i][2];

		for (int j = 0; j < 3; j++)
			v3dSum(vnormals[ids[j]], fnormals[i], vnormals[ids[j]]);
	}

	/* Normalizzo le normali ai vertici */
	for (int i = 0; i < nvert; i++) {
		for (int j = 0; j < 3; j++) {
			vnormals[i][j] /= vertOccurrences[i];
		}
	}

	// crea la display list mesh
	printf("Creazione display list .. \n");
	//listname = glGenLists(1);  //ID della lista è listname
	glNewList(listname, GL_COMPILE);
	
	for (int i = 0; i < nface; i++)
	{
		ids[2] = faces[i][0];
		ids[1] = faces[i][1];
		ids[0] = faces[i][2];

		//disegna triangoli coi vertici specificati
		glNormal3f(fnormals[i][0], fnormals[i][1], fnormals[i][2]);
		glColor3f(1, 0, 0);
		glBegin(GL_TRIANGLES);
		for (int ii = 2; ii >= 0; ii--)
		{
			glNormal3f(vnormals[ids[ii]][0], vnormals[ids[ii]][1], vnormals[ids[ii]][2]);
			glVertex3f(vertices[ids[ii]][0], vertices[ids[ii]][1], vertices[ids[ii]][2]);
		}
		glEnd();
	}

	glEndList();
}

void init()
{
	plotModel("data/cactus.m", 1);
	plotModel("data/pig.m", 2);
	plotModel("data/teapot.m", 3);

	myReusableQuadric = gluNewQuadric();  
	gluQuadricNormals( myReusableQuadric, GL_TRUE );

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);

	glClearColor( 1.0, 1.0, 1.0, 1.0 );

	//glEnable (GL_POINT_SMOOTH);
	//glEnable (GL_LINE_SMOOTH);
	glHint (GL_POINT_SMOOTH_HINT, GL_NICEST);	// Make round points, not square points
	glHint (GL_LINE_SMOOTH_HINT, GL_NICEST);	// Antialias the lines
	glEnable (GL_BLEND);
	//glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	init_CV();
}

void print_sys_status()
{
	system("clear");
	printf("\nSystem status\n\n");
	printf("---------------------------------------------------\n");
	printf("Eye point coordinates :\n");
	printf("x = %.2f, y = %.2f, z = %.2f\n", camE[0], camE[1], camE[2]);
	printf("---------------------------------------------------\n");
	printf("Reference point coordinates :\n");
	printf("x = %.2f, y = %.2f, z = %.2f\n", camC[0], camC[1], camC[2]);
	printf("---------------------------------------------------\n");
	printf("Up vector coordinates :\n");
	printf("x = %.2f, y = %.2f, z = %.2f\n", camU[0], camU[1], camU[2]);
	printf("---------------------------------------------------\n");
	printf("Light position coordinates :\n");
	printf("x = %.2f, y = %.2f, z = %.2f\n", lightPos[0], lightPos[1], lightPos[2]);
	printf("---------------------------------------------------\n");
	printf("Axis rotation (in degree) :\n");
	printf("x = %.2f, y = %.2f, z = %.2f\n", angle[0], angle[1], angle[2]);
	printf("---------------------------------------------------\n");
	printf("Field of view angle (in degree) = %.2f\n", fovy);
	printf("---------------------------------------------------\n");
	if(wireframe)
		printf("Wireframe = YES\n");
	else
		printf("Wireframe = NO\n");
	printf("---------------------------------------------------\n");	
}

void menu(int sel)
{
	if(sel == MODE_CHANGE_EYE_POS || sel == MODE_CHANGE_REFERENCE_POS || sel == MODE_CHANGE_UP_POS || sel == MODE_CHANGE_LIGHT_POS || sel == MODE_ROTATE_WORLD )
		mode = sel;
	if(sel == MODE_CHANGE_CULLING)
		cull = !cull;
	if(sel == MODE_CHANGE_WIREFRAME)
		wireframe = !wireframe;
	if(sel == MODE_CHANGE_PROJECTION)
		orpro = !orpro;
	if(sel == MODE_CHANGE_SHADING)
		shading = !shading;
	if(sel == MODE_CHANGE_MATERIAL )
		mater = (mater+1)%4; 
	if(sel == MODE_CHANGE_ZOOM)
		zoom = !zoom;
	if(sel == MODE_RESET)
		reset();
	if(sel == MODE_QUIT)
		exit(0);
	if(sel == MODE_PRINT_SYSTEM_STATUS)
		print_sys_status();
	if (sel == MODE_ANIMATION)
		animation = !animation;
	if (sel == MODE_ROTATE_OCS || sel == MODE_TRANSLATE_OCS || sel == MODE_ROTATE_WCS || sel == MODE_TRANSLATE_WCS || sel == MODE_ROTATE_VCS || sel == MODE_TRANSLATE_VCS)
		mode = sel;
	glutPostRedisplay();
}

int main (int argc, char** argv)
{
    glutInit(&argc,argv);
    glutInitDisplayMode( GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE );
    glutInitWindowSize( WindowWidth, WindowHeight ); 
    glutInitWindowPosition (0, 0);
    glutCreateWindow("Model Viewer");

    glutDisplayFunc(display);
	glutSpecialFunc(special);		// frecce up, down, left, right
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);

	glutCreateMenu(menu); 
	glutAddMenuEntry("Menu",-1); //-1 significa che non si vuole gestire questa riga
	glutAddMenuEntry("",-1);
	glutAddMenuEntry("Change eye point (use x,y,z,X,Y,Z)", MODE_CHANGE_EYE_POS);
	glutAddMenuEntry("Change reference point (use x,y,z,X,Y,Z)", MODE_CHANGE_REFERENCE_POS);
	glutAddMenuEntry("Change up vector (use x,y,z,X,Y,Z)", MODE_CHANGE_UP_POS);
	glutAddMenuEntry("Change light position (use x,y,z,X,Y,Z)", MODE_CHANGE_LIGHT_POS);
	glutAddMenuEntry("Zoom (use f,F)", MODE_CHANGE_ZOOM);
	glutAddMenuEntry("Rotate world (use x,y,z,X,Y,Z)", MODE_ROTATE_WORLD );

	glutAddMenuEntry("",-1);
	glutAddMenuEntry("Culling", MODE_CHANGE_CULLING);      
	glutAddMenuEntry("Wireframe", MODE_CHANGE_WIREFRAME);
	glutAddMenuEntry("Ortographic or Prospective", MODE_CHANGE_PROJECTION); 
	glutAddMenuEntry("Shading", MODE_CHANGE_SHADING); 
	glutAddMenuEntry("Material", MODE_CHANGE_MATERIAL); 
	glutAddMenuEntry("Animation (use a)", MODE_ANIMATION);

	glutAddMenuEntry("", -1);
	glutAddMenuEntry("Rotate OCS", MODE_ROTATE_OCS);
	glutAddMenuEntry("Translate OCS", MODE_TRANSLATE_OCS);
	glutAddMenuEntry("Rotate WCS", MODE_ROTATE_WCS);
	glutAddMenuEntry("Translate WSC", MODE_TRANSLATE_WCS);
	glutAddMenuEntry("Rotate VCS", MODE_ROTATE_VCS);
	glutAddMenuEntry("Translate VCS", MODE_TRANSLATE_VCS);

	glutAddMenuEntry("",-1);   
	glutAddMenuEntry("Print system status", MODE_PRINT_SYSTEM_STATUS);    
	glutAddMenuEntry("Reset", MODE_RESET);     
	glutAddMenuEntry("Quit", MODE_QUIT);

	glutAttachMenu(GLUT_RIGHT_BUTTON);

	reset();
    init();

    glutMainLoop();

	return -1;
}

void findMinMax(float a, float b, float c) {
	if (a < orthoLeft)
		orthoLeft = a;
	if (a > orthoRight)
		orthoRight = a;
	if (b < orthoBottom)
		orthoBottom = b;
	if (b > orthoTop)
		orthoTop = b;
	if (c < orthoNear)
		orthoNear = c;
	if (c > orthoFar)
		orthoFar = c;
}

float maxDistance() {
	float max = orthoBottom - orthoTop + 4.0;
	float width = orthoRight - orthoLeft + 4.0;
	float depth = orthoFar - orthoNear + 4.0;
	if (max < width)
		max = width;
	if (max < depth)
		max = depth;
	return max * 10;
}

float lerp(float x0, float x1, float t) {
	return (1 - t) * x0 + t * x1;
}

void de_casteljau(GLfloat t)
{
	for (int i = 0; i < MAX_CV; i++)
		for (int j = 0; j < 2; j++)
			CJ[i][j] = CV[i][j];

	for (int i = 1; i < 5 ; i++)
	{
		for (int j = 0; j < 5 - i; j++)
		{
			CJ[j][0] = lerp(CJ[j][0], CJ[j + 1][0], t);
			CJ[j][1] = lerp(CJ[j][1], CJ[j + 1][1], t);
		}
	}
}

void init_CV() {

	float varx = camE[0];
	float vary = camE[1];

	CV[0][0] = varx;
	CV[0][1] = vary;

	CV[1][0] = 120;
	CV[1][1] = -60;

	CV[2][0] =  100;
	CV[2][1] =  160;

	CV[3][0] =	-60;
	CV[3][1] =  120;

	CV[4][0] = varx;
	CV[4][1] = vary;
}

void callDC() {
	if (animation) {
		de_casteljau(stepA);
		camE[0] = CJ[0][0];
		camE[1] = CJ[0][1];
	}
}
