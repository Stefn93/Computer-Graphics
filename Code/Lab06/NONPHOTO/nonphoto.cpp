#include "../common/common.h"

GLuint program = 0;

static void init()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double) glutGet(GLUT_WINDOW_WIDTH) / (double) glutGet(GLUT_WINDOW_HEIGHT), 1.0, 20.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    const GLfloat lightPos[4] = {3.0f, 3.0f, 3.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);

    glewInit();
    program = initShader("v.glsl", "f.glsl");
}

static void draw(void)
{
    double t = (double) glutGet(GLUT_ELAPSED_TIME);
    double k = 0.05 * 360.0 / 1000.0;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
        glTranslatef(0.0f, 0.0f, -5.0f);
        glRotatef(k*t, 1.0, 0.0, 0.0);
        glRotatef(k*t, 0.0, 1.0, 0.0);
        glutSolidTeapot(1.0);
    glPopMatrix();

    glutSwapBuffers();
    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutCreateWindow("Simple GLSL example");
    glutDisplayFunc(draw);
    glutKeyboardFunc(commonKeyboard);

    init();

    glutMainLoop();
    return 0;

}
