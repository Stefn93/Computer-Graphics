#include "../common/common.h"

GLuint         program;
GLint          timeParam;
GLint          vertices_two_location;
GLint          vertices_three_location;

const GLfloat vertices_one[3][2] = {{0.0, 0.0},{0.5,1.0},{1.0, 0.0}};
const GLfloat vertices_two[3][2] = {{0.0, 1.0},{0.5,0.0},{1.0, 1.0}};
//const GLfloat vertices_three[4][2] = {{0.0, 0.0}, { 0.0, 1.0 }, { 1.0, 1.0 }, { 1.0, 0.0 }};

static void init()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glColor3f(0.0,0.0,0.0);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0,1.0,0.0,1.0);

    glEnable(GL_DEPTH_TEST);

    glewInit();
    program = initShader("v.glsl", "f.glsl");

    // Setup uniform and attribute parameters
    timeParam = glGetUniformLocation(program, "time");
    vertices_two_location = glGetAttribLocation(program, "vertices2");
	//vertices_three_location = glGetAttribLocation(program, "vertices3");
	/* ----------- ADD ------------------ 
			handle additional variables if needed
	*/
}

static void draw(void)
{
    /* send elapsed time to shaders */
    glUniform1f(timeParam, glutGet(GLUT_ELAPSED_TIME));

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
	//glBegin(GL_LINE_LOOP);
		/* ----------- ADD ------------------ 
			instead of lines, draw a triangle
			assign color to the vertices
		*/
        /*glVertexAttrib2fv(vertices_two_location, &vertices_two[0][0]);
        glVertex2fv(vertices_one[0]);
        glVertexAttrib2fv(vertices_two_location, &vertices_two[1][0]);
        glVertex2fv(vertices_one[1]);
        glVertexAttrib2fv(vertices_two_location, &vertices_two[2][0]);
        glVertex2fv(vertices_one[2]);*/
	glBegin(GL_TRIANGLES);
		glColor3f(1, 0, 0);
		glVertexAttrib2fv(vertices_two_location, &vertices_two[0][0]);
		glVertex2fv(vertices_one[0]);
		glColor3f(0, 1, 0);
		glVertexAttrib2fv(vertices_two_location, &vertices_two[1][0]);
		glVertex2fv(vertices_one[1]);
		glColor3f(0, 0, 1);
		glVertexAttrib2fv(vertices_two_location, &vertices_two[2][0]);
		glVertex2fv(vertices_one[2]);
		//glVertexAttrib2fv(vertices_three_location, &vertices_three[0][0]);
		//glVertexAttrib2fv(vertices_three_location, &vertices_three[1][0]);
		//glVertexAttrib2fv(vertices_three_location, &vertices_three[2][0]);
		//glVertexAttrib2fv(vertices_three_location, &vertices_three[3][0]);
	glEnd();
    //glEnd();

    glutSwapBuffers();
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

    init();

    glutMainLoop();
    return 0;
}
