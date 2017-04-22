#include "../Include/Common.h"
#include "modelData.h"

#define MENU_TIMER_START 1
#define MENU_TIMER_STOP 2
#define MENU_NEGATIVE_ON 4
#define MENU_NEGATIVE_OFF 5
#define MENU_EXIT 3

GLubyte timer_cnt = 0;
bool timer_enabled = true;
unsigned int timer_speed = 16;

using namespace glm;
using namespace std;

mat4 mvp;
GLint um4mvp, myLoc;

GLuint program;

char** loadShaderSource(const char* file)
{
    FILE* fp = fopen(file, "rb");
    fseek(fp, 0, SEEK_END);
    long sz = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *src = new char[sz + 1];
    fread(src, sizeof(char), sz, fp);
    src[sz] = '\0';
    char **srcp = new char*[1];
    srcp[0] = src;
    return srcp;
}

void freeShaderSource(char** srcp)
{
    delete srcp[0];
    delete srcp;
}

// TODO : 
// (1.) Generate a uniform int flag
// (2.) Generate buffer to pass the data into GPU
GLfloat* vertice;
GLfloat* color;
void se()
{
	vertice = new float[24876/2];
	color = new float[24876/2];
	int j = 0;
	for (int i = 0; i < 24876; i+=6) {
		vertice[j] = modelData[i];
		color[j] = modelData[i+3];
		j++;
		vertice[j] = modelData[i + 1];
		color[j] = modelData[i + 4];
		j++;
		vertice[j] = modelData[i + 2];
		color[j] = modelData[i + 5];
		j++;
	}
}

void rr()
{
	for (int i = 0; i < 20; i++) {
		printf("%f ", modelData[i]);
	}
	printf("\n");
	for (int i = 0; i < 10; i++) {
		printf("%f ", vertice[i]);
	}
	printf("\n");
	for (int i = 0; i < 10; i++) {
		printf("%f ", color[i]);
	}
	printf("\n");
}

void My_Init()
{
	se();
	rr();
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

    program = glCreateProgram();
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	char** vertexShaderSource = loadShaderSource("vertex.vs.glsl");
	char** fragmentShaderSource = loadShaderSource("fragment.fs.glsl");
    glShaderSource(vertexShader, 1, vertexShaderSource, NULL);
    glShaderSource(fragmentShader, 1, fragmentShaderSource, NULL);
	freeShaderSource(vertexShaderSource);
	freeShaderSource(fragmentShaderSource);
    glCompileShader(vertexShader);
    glCompileShader(fragmentShader);
	shaderLog(vertexShader);
    shaderLog(fragmentShader);
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
	um4mvp = glGetUniformLocation(program, "um4mvp");
	myLoc = glGetUniformLocation(program, "flag");
    glUseProgram(program);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	// bind array pointers to shader
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vertice);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, color);

	
}

// TODO : Sending a draw command here
void My_Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUniformMatrix4fv(um4mvp, 1, GL_FALSE, value_ptr(mvp));
	glDrawArrays(GL_TRIANGLES, 0, 4146);

    glutSwapBuffers();
}

void My_Reshape(int width, int height)
{
	glViewport(0, 0, width, height);

	float viewportAspect = (float)width / (float)height;
	mvp = ortho(-20 * viewportAspect, 20 * viewportAspect, -20.0f, 20.0f, 0.0f, 50.0f) * lookAt(vec3(0.0f, 20.0f, 20.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f)) * translate(mat4(), vec3(0, 0, 10));
}

void My_Timer(int val)
{
	timer_cnt++;
	glutPostRedisplay();
	if(timer_enabled)
	{
		glutTimerFunc(timer_speed, My_Timer, val);
	}
}

void My_Mouse(int button, int state, int x, int y)
{
	if(state == GLUT_DOWN)
	{
		printf("Mouse %d is pressed at (%d, %d)\n", button, x, y);
	}
	else if(state == GLUT_UP)
	{
		printf("Mouse %d is released at (%d, %d)\n", button, x, y);
	}
}

void My_Keyboard(unsigned char key, int x, int y)
{
	printf("Key %c is pressed at (%d, %d)\n", key, x, y);
}

void My_SpecialKeys(int key, int x, int y)
{
	switch(key)
	{
	case GLUT_KEY_F1:
		printf("F1 is pressed at (%d, %d)\n", x, y);
		break;
	case GLUT_KEY_PAGE_UP:
		printf("Page up is pressed at (%d, %d)\n", x, y);
		break;
	case GLUT_KEY_LEFT:
		printf("Left arrow is pressed at (%d, %d)\n", x, y);
		break;
	default:
		printf("Other special key is pressed at (%d, %d)\n", x, y);
		break;
	}
}


// TODO : Pass uniform int flag to fragment shader here
void My_Menu(int id)
{
	switch (id)
	{
	case MENU_TIMER_START:
		if (!timer_enabled)
		{
			timer_enabled = true;
			glutTimerFunc(timer_speed, My_Timer, 0);
		}
		break;
	case MENU_TIMER_STOP:
		timer_enabled = false;
		break;
	case MENU_NEGATIVE_ON:
		glUniform1i(myLoc, 1);
		break;
	case MENU_NEGATIVE_OFF:
		glUniform1i(myLoc, 0);
		break;
	case MENU_EXIT:
		exit(0);
		break;
	default:
		break;
	}
}

int main(int argc, char *argv[])
{
#ifdef __APPLE__
    // Change working directory to source code path
    chdir(__FILEPATH__("/../Assets/"));
#endif
	// Initialize GLUT and GLEW, then create a window.
	////////////////////
	glutInit(&argc, argv);
#ifdef _MSC_VER
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#else
    glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#endif
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 800);
	glutCreateWindow("Quiz 2"); // You cannot use OpenGL functions before this line; The OpenGL context must be created first by glutCreateWindow()!
#ifdef _MSC_VER
	glewInit();
#endif
	dumpInfo();
	My_Init();

	// Create a menu and bind it to mouse right button.
	int menu_main = glutCreateMenu(My_Menu);
	int menu_timer = glutCreateMenu(My_Menu);

	glutSetMenu(menu_main);
	glutAddSubMenu("Timer", menu_timer);
	glutAddMenuEntry("Negative On", MENU_NEGATIVE_ON);
	glutAddMenuEntry("Negative Off", MENU_NEGATIVE_OFF);
	glutAddMenuEntry("Exit", MENU_EXIT);

	glutSetMenu(menu_timer);
	glutAddMenuEntry("Start", MENU_TIMER_START);
	glutAddMenuEntry("Stop", MENU_TIMER_STOP);

	glutSetMenu(menu_main);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	// Register GLUT callback functions.
	glutDisplayFunc(My_Display);
	glutReshapeFunc(My_Reshape);
	glutMouseFunc(My_Mouse);
	glutKeyboardFunc(My_Keyboard);
	glutSpecialFunc(My_SpecialKeys);
	glutTimerFunc(timer_speed, My_Timer, 0); 

	// Enter main event loop.
	glutMainLoop();

	return 0;
}