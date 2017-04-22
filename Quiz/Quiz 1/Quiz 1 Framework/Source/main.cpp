#include "../Include/Common.h"

//For GLUT to handle 
#define MENU_TIMER_START 1
#define MENU_TIMER_STOP 2
#define MENU_EXIT 3

//GLUT timer variable
GLubyte timer_cnt = 0;
bool timer_enabled = true;
unsigned int timer_speed = 16;

GLint myLoc;
int judge = 0;

using namespace glm;
using namespace std;

mat4 view;					// V of MVP, viewing matrix
mat4 projection;			// P of MVP, projection matrix
mat4 model;					// M of MVP, model matrix

GLint um4mvp;			// shader mvp uniform id 
GLuint program;			// shader program id

						// Load shader file to program
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

// Free shader file
void freeShaderSource(char** srcp)
{
	delete srcp[0];
	delete srcp;
}

// OpenGL initialization
void My_Init()
{
	// Clear display windows color to white color(1.0,1.0,1.0,1.0)
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	// Enable depth test, will not draw pixels that are blocked by other pixels 
	glEnable(GL_DEPTH_TEST);

	// Set the type of depth test
	glDepthFunc(GL_LEQUAL);

	// Create Shader Program
	program = glCreateProgram();

	// Create customize shader by tell openGL specify shader type 
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	// Load shader file
	char** vertexShaderSource = loadShaderSource("vertex.vs.glsl");
	char** fragmentShaderSource = loadShaderSource("fragmentz.fs.glsl");

	// Assign content of these shader files to those shaders we created before
	glShaderSource(vertexShader, 1, vertexShaderSource, NULL);
	glShaderSource(fragmentShader, 1, fragmentShaderSource, NULL);

	// Free the shader file string(won't be used any more)
	freeShaderSource(vertexShaderSource);
	freeShaderSource(fragmentShaderSource);

	// Compile these shaders
	glCompileShader(vertexShader);
	glCompileShader(fragmentShader);

	// Logging
	shaderLog(vertexShader);
	shaderLog(fragmentShader);

	// Assign the program we created before with these shaders
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);

	// Get the id of inner variable 'um4mvp' in shader programs
	um4mvp = glGetUniformLocation(program, "um4mvp");

	// Tell OpenGL to use this shader program now
	glUseProgram(program);

	//Create vertex array object and bind it to  OpenGL (OpenGL will apply operation only to the vertex array objects it bind)
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//Enable shader layout location 0 and 1 for vertex and color   
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	//Create buffer and bind it to OpenGL
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	myLoc = glGetUniformLocation(program, "i");
}

// GLUT callback. Called to draw the scene.
void My_Display()
{
	// Clear display buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Create vertex and color data
	float data[18] = {

		-0.5f, -0.4f, 0.0f,
		0.5f, -0.4f, 0.0f,
		0.0f,  0.6f, 0.0f,

		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
	};

	//Put data into buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

	//Select which range as vertex and which as color
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float) * 9));

	// Tell openGL to use the shader program we created before
	glUseProgram(program);

	//Calculate mvp matrix by ( projection * view * model ) 
	mat4 mvp = projection * view * model;

	// Transfer value of mvp to both shader's inner variable 'um4mvp'; 
	glUniformMatrix4fv(um4mvp, 1, GL_FALSE, value_ptr(mvp));

	// Tell openGL to draw the vertex array we had binded before
	glDrawArrays(GL_TRIANGLES, 0, 3);

	// Change current display buffer to another one (refresh frame) 
	glutSwapBuffers();
}

// Setting up viewing matrix
// This function will be called whenever the size of windows is changing
void My_Reshape(int width, int height)
{
	// Setting where to display and size of OpenGL display area
	glViewport(0, 0, width, height);

	float viewportAspect = (float)width / (float)height;
	projection = ortho(-1 * viewportAspect, 1 * viewportAspect, -1.0f, 1.0f);
	view = lookAt( vec3(0.0f,0.0f,1.0f) , vec3(0.0f,0.0f,0.0f) , vec3(0.0f,1.0f,0.0f) );
}


void My_Timer(int val)
{
	timer_cnt++;
	glutPostRedisplay();
	if (timer_enabled)
	{
		glutTimerFunc(timer_speed, My_Timer, val);
	}
}

void My_Keyboard(unsigned char key, int x, int y)
{
	printf("Key %c is pressed at (%d, %d)\n", key, x, y);
}

void My_SpecialKeys(int key, int x, int y)
{
	switch (key)
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
	case MENU_EXIT:
		exit(0);
		break;
	default:
		break;
	}
}

void My_Mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			judge++;
			if (judge > 1) judge = 0;
			if(judge==0) glUniform1i(myLoc, 0);
			else glUniform1i(myLoc, 1);


			printf("Mouse %d is pressed at (%d, %d)\n", button, x, y);
		}
		else if (state == GLUT_UP)
		{

			printf("Mouse %d is released at (%d, %d)\n", button, x, y);
		}
	}
}

int main(int argc, char *argv[])
{
#ifdef __APPLE__
	// Change working directory to source code path
	chdir(__FILEPATH__("/../Assets/"));
#endif
	// Initialize GLUT and GLEW, then create a window.
	glutInit(&argc, argv);
#ifdef _MSC_VER
	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
#else
	glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#endif
	
	glutInitWindowPosition( 100 ,100 );
	glutInitWindowSize( 600, 600);
	glutCreateWindow("Quiz 1");


#ifdef _MSC_VER
	glewInit();
#endif
	dumpInfo();
	My_Init();

	int menu_main = glutCreateMenu(My_Menu);
	int menu_timer = glutCreateMenu(My_Menu);
	int menu_speed = glutCreateMenu(My_Menu);

	glutSetMenu(menu_main);
	glutAddSubMenu("Timer", menu_timer);
	glutAddSubMenu("Speed", menu_speed);
	glutAddMenuEntry("Exit", MENU_EXIT);

	glutSetMenu(menu_timer);
	glutAddMenuEntry("Start", MENU_TIMER_START);
	glutAddMenuEntry("Stop", MENU_TIMER_STOP);

	glutSetMenu(menu_main);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	// Register GLUT callback functions.
	glutDisplayFunc(My_Display);
	glutReshapeFunc(My_Reshape);
	glutKeyboardFunc(My_Keyboard);
	glutSpecialFunc(My_SpecialKeys);
	glutTimerFunc(timer_speed, My_Timer, 0);
	glutMouseFunc(My_Mouse);

	// Enter main event loop.
	glutMainLoop();

	return 0;
}