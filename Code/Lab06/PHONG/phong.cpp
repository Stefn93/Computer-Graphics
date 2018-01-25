#include "../common/common.h"

GLuint program[2];

//vedi slides phong lighting - f1 e v1 fanno il calcolo vertice per vertice
//phong shading - applicare phong lighting per ogni FRAMMENTO - using halfway vector (modello di bleene) - v*r^a viene sostituito come sulle slides (già fatto in v2 e f2)
static void init()
{
    const float teapotColor[]     = {0.0f, 0.3f, 0.6f, 1.0f};
    const float teapotSpecular[]  = {0.5f, 1.0f, 1.0f, 1.0f};
    const float teapotShininess[] = {80.0f};

    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, teapotColor);
    glMaterialfv(GL_FRONT, GL_SPECULAR, teapotSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, teapotShininess);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double) glutGet(GLUT_WINDOW_WIDTH) / (double) glutGet(GLUT_WINDOW_HEIGHT), 1.0, 20.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    const GLfloat  lightPos[4] = {3.0f, 3.0f, 3.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);

    glewInit();

    program[0] = initShader("v1.glsl", "f1.glsl");
    program[1] = initShader("v2.glsl", "f2.glsl");
	program[2] = initShader("v3.glsl", "f3.glsl");

    glUseProgram(program[0]);
}

static void draw(void)
{
    double t = (double)glutGet(GLUT_ELAPSED_TIME);
	double k = 0; // 0.05 * 360.0 / 1000.0;  //ripristina per la rotazione delle teapot

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(program[1]);
    glPushMatrix();
        glTranslatef(0.0f, 0.0f, -5.0f);
        glRotatef(k*t, 1.0, 0.0, 0.0);
        glRotatef(k*t, 0.0, 1.0, 0.0);
        glutSolidTeapot(1.0);
    glPopMatrix();

	//1
	glUseProgram(program[2]);
	glPushMatrix();
		glTranslatef(3.0f, 0.0f, -5.0f);
		glRotatef(k*t, 1.0, 0.0, 0.0);
		glRotatef(k*t, 0.0, 1.0, 0.0);
		glutSolidTeapot(1.0);
	glPopMatrix();

	//2
	glUseProgram(program[0]);
	glPushMatrix();
		glTranslatef(-3.0f, 0.0f, -5.0f);
		glRotatef(k*t, 1.0, 0.0, 0.0);
		glRotatef(k*t, 0.0, 1.0, 0.0);
		glutSolidTeapot(1.0);
	glPopMatrix();

    glutSwapBuffers();
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
    commonKeyboard(key, x, y);
	if (key == '1')
		glUseProgram(program[0]);
	else if (key == '2')
		glUseProgram(program[1]);
	else if (key == '3')
		glUseProgram(program[2]);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(1366, 512);
    glutCreateWindow("Simple GLSL example");
    glutDisplayFunc(draw);
    glutKeyboardFunc(keyboard);

    init();

    glutMainLoop();
    return 0;
}
