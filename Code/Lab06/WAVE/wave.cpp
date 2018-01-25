/* sets up flat mesh */
/* sets up elapsed time parameter for use by shaders */
/* vertex shader varies height of mesh sinusoidally */
/* uses a pass through fragment shader */

#include "../common/common.h"

#define N 64

GLuint program;
GLint timeParam;
GLint amplitude;
GLint frequency;

float amps[] = { 0.05, 0.1, 0.2 };
float freqs[] = { 0.0005, 0.001, 0.002 };

int actuala = 0;
int actualf = 0;

static void init()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glColor3f(0.0, 0.0, 0.0);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-0.75,0.75,-0.75,0.75,-5.5,5.5);

    glewInit();
    program = initShader("v.glsl", "f.glsl");

	//
	timeParam = glGetUniformLocation(program, "time"); //la chiamerò time nel vertex shader
	amplitude = glGetUniformLocation(program, "A");
	frequency = glGetUniformLocation(program, "omega");

	glUniform1f(amplitude, 0.05);
	glUniform1f(frequency, 0.001);
	//spostarle nella funzione del mouse facendole variare a seconda dei click

	/* ----------- ADD ------------------ 
	glGetUniformLocation of time, amplitude and frequency

	set the values via glUniform1f
	*/
}

void mesh()
{
    int i,j;
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(2.0, 2.0, 2.0, 0.5, 0.0, 0.5, 0.0, 1.0, 0.0);
    for(i=0; i<N; i++)
    {
        for(j=0; j<N;j++)
        {
            glColor3ub(50,109,164);
            glBegin(GL_QUADS);
                glVertex3f((float)i/N, 0.0, (float)j/N);
                glVertex3f((float)i/N, 0.0, (float)(j+1)/N);
                glVertex3f((float)(i+1)/N, 0.0, (float)(j+1)/N);
                glVertex3f((float)(i+1)/N, 0.0, (float)(j)/N);
            glEnd();

            glColor3f(0.2, 0.2, 0.2);
            glBegin(GL_LINE_LOOP);
                glVertex3f((float)i/N, 0.0, (float)j/N);
                glVertex3f((float)i/N, 0.0, (float)(j+1)/N);
                glVertex3f((float)(i+1)/N, 0.0, (float)(j+1)/N);
                glVertex3f((float)(i+1)/N, 0.0, (float)(j)/N);
            glEnd();
        }
    }
}

static void draw(void)
{
	/* ----------- ADD ------------------ 
		send elapsed time to shaders

		to get the elpased time, use glutGet(GLUT_ELAPSED_TIME)
	*/
	glUniform1f(timeParam, glutGet(GLUT_ELAPSED_TIME));
	//
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    mesh();

    glutSwapBuffers();
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)
{
	if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN)) {
		actuala++;
		actuala %= 3;
		glUniform1f(amplitude, amps[actuala]);
	}

	if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_DOWN)) {
		actualf++;
		actualf %= 3;
		glUniform1f(frequency, freqs[actualf]);
	}
}

int main(int argc, char** argv)
{
    srand(time(0));

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    glutInitWindowSize(512, 512);
    glutCreateWindow("Simple GLSL example");
    glutDisplayFunc(draw);
    glutKeyboardFunc(commonKeyboard);
    glutMouseFunc(mouse);

    init();

    glutMainLoop();
}
