#include "../Include/Common.h"

//For GLUT to handle 
#define MENU_TIMER_START 1
#define MENU_TIMER_STOP 2
#define MENU_EXIT 3
#define MENU_SLOW 4
#define MENU_FAST 5

//GLUT timer variable
GLubyte timer_cnt = 0;
bool timer_enabled = true;
unsigned int timer_speed = 16;

using namespace glm;
using namespace std;

vec3 temp = vec3();
int judge = 0;
float degree = 0.03;
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
	char** fragmentShaderSource = loadShaderSource("fragment.fs.glsl");

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

	//TODO :
	//Create rotate and translate martix and calculate model matrix

	//Build translation matrix
	mat4 translation_matrix = translate(mat4(), temp);
	temp = vec3();
	//Build rotation matrix
	vec3 rotate_axis = vec3(0.0, 0.0, 1.0);
	mat4 rotation_matrix = rotate(model, degree, rotate_axis);

	model = translation_matrix*rotation_matrix;

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

	// TODO :
	// Building a projection matrix by calling perspective
	// perspective(fov, aspect_ratio, near_plane_distance, far_plane_distance)
	// fov = field of view, it represent how much range(degree) is this camera could see 
	// projection = perspective( ... , ... , ... );
	projection = perspective(radians(60.0f), viewportAspect, 0.1f, 1000.0f);
	
	// TODO :
	// Building a viewing matrix by calling lookAt
	// lookAt(camera_position, camera_viewing_vector, up_vector)
	// up_vector represent the vector which define the direction of 'up'
	// view = lookAt( ... , ... , ... );
	view = lookAt(vec3(0.0f, 0.0f, 3.0f), vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, 1.0f, 0.0f));

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
	//TODO :
	//When button down translate the triangle 

	printf("Key %c is pressed at (%d, %d)\n", key, x, y);
	if (key == 'd')
	{
		temp = temp + vec3(0.4, 0, 0);
	}
	else if (key == 'a')
	{
		temp = temp - vec3(0.4, 0, 0);
	}
	else if (key == 'w')
	{
		temp = temp + vec3(0, 0.4, 0);
	}
	else if (key == 's')
	{
		temp = temp - vec3(0, 0.4, 0);
	}
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

		//TODO :
		//You should add new case to deal with MENU_SLOW and MENU_FAST
	case MENU_SLOW:
		degree = 0.01;
		break;
	case MENU_FAST:
		degree = 0.05;
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
			printf("Mouse %d is pressed at (%d, %d)\n", button, x, y);
		}
		else if (state == GLUT_UP)
		{
			printf("Mouse %d is released at (%d, %d)\n", button, x, y);
		}
	}
}

void My_newMenu(int id)
{
	
}

int main(int argc, char *argv[])
{
#ifdef __APPLE__
	// Change working directory to source code path
	chdir(__FILEPATH__("/../Assets/"));
#endif
	// Initialize GLUT and GLEW, then create a window.
	glutInit(&argc, argv);

	// TODO :
	// Setting display mode for GLUT here
#ifdef _MSC_VER
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	//glutInitDisplayMode(... | ... | ...);
#else
	glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#endif
	
	// TODO :
	// Creating windows here (remeber to set both size & position for it )
	glutInitWindowPosition(500, 100);
	glutInitWindowSize(600, 600);
	glutCreateWindow("Practice2");
	//glutInitWindowPosition( ... , ... );
	//glutInitWindowSize( ... , ... );
	//glutCreateWindow( ... );


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

	//TODO :
	//Create new entry about MENU_SLOW and MENU_FAST under the speed menu
	glutSetMenu(menu_speed);
	glutAddMenuEntry("Slow", MENU_SLOW);
	glutAddMenuEntry("Fast", MENU_FAST);



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