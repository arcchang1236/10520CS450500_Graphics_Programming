#include "../../Include/Common.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923
#endif


using namespace glm;
mat4 mv5_hand_rotate;
mat4 mv6_hand_rotate;
mat4 mv4_leg_rotate;
mat4 mv3_leg_rotate;
mat4 head_rotate;
int c = 0;
float sum = 0;
float r;
float w_head, w_hand, w_leg;
bool timer_enable = true;

using namespace std;

static const char * vs_source[] =
{
	"#version 410 core                                                  \n"
	"                                                                   \n"
	"in vec4 position;                                                  \n"
	"                                                                   \n"
	"out VS_OUT                                                         \n"
	"{                                                                  \n"
	"    vec4 color;                                                    \n"
	"} vs_out;                                                          \n"
	"                                                                   \n"
	"uniform mat4 mv_matrix;                                            \n"
	"                                                                   \n"
	"void main(void)                                                    \n"
	"{                                                                  \n"
	"    gl_Position =  mv_matrix * position;					        \n"
	"    vs_out.color = position * 2.0 + vec4(0.5, 0.5, 0.5, 0.0);      \n"
	"}                                                                  \n"
};

static const char * fs_source[] =
{
	"#version 410 core                                                  \n"
	"                                                                   \n"
	"out vec4 color;                                                     \n"
	"                                                                   \n"
	"in VS_OUT                                                          \n"
	"{                                                                  \n"
	"    vec4 color;                                                    \n"
	"} fs_in;                                                           \n"
	"                                                                   \n"
	"void main(void)                                                    \n"
	"{                                                                  \n"
	"    color = fs_in.color;                                           \n"
	"}                                                                  \n"
};

std::vector<GLfloat> vertices;
std::vector<GLushort> indices;
GLfloat* sphereVertex;
GLfloat* sphereColors;
void putData()
{
	sphereVertex = new GLfloat[indices.size() * 3];
	sphereColors = new GLfloat[indices.size() * 3];
	int i = 0;
	vector<GLushort>::iterator tmp;
	for (tmp = indices.begin(); tmp != indices.end(); tmp++) {
		//cout<<*tmp<<endl;
		sphereVertex[i] = vertices[(*tmp) * 3];
		sphereVertex[i + 1] = vertices[(*tmp) * 3 + 1];
		sphereVertex[i + 2] = vertices[(*tmp) * 3 + 2];
		sphereColors[i] = 0.583f;
		sphereColors[i + 1] = 0.592f;
		sphereColors[i + 2] = 0.520f;
		i = i + 3;
	}
	cout << indices.size() << endl;
	cout << vertices.size() << endl;
}
void createSphere(float radius, unsigned int rings, unsigned int sectors)
{
	float const R = 1. / (float)(rings - 1);
	float const S = 1. / (float)(sectors);
	int r, s;

	vertices.resize(rings * sectors * 3);
	std::vector<GLfloat>::iterator v = vertices.begin();

	for (r = 0; r < rings; r++)
		for (s = 0; s < sectors; s++) {
			float const y = sin(-M_PI_2 + M_PI * r * R);
			float const x = cos(2 * M_PI * s * S) * sin(M_PI * r * R);
			float const z = sin(2 * M_PI * s * S) * sin(M_PI * r * R);

			*v++ = x * radius;
			*v++ = y * radius;
			*v++ = z * radius;
		}

	indices.resize((rings - 3) * sectors * 6 + 2 * sectors * 3);
	std::vector<GLushort>::iterator i = indices.begin();
	int num = 0;
	for (r = 0; r < rings - 1; r++) {
		for (s = 0; s < sectors; s++) {
			if (r == 0) {
				*i++ = r * sectors + s;
				*i++ = r * sectors + ((s + 1) % sectors);
				*i++ = (r + 1) * sectors + s;
			}
			else if (r == rings - 2) {
				*i++ = (r + 1) * sectors + s;
				*i++ = (r + 1) * sectors + ((s + 1) % sectors);
				*i++ = r * sectors + ((s + 1) % sectors);

			}
			else {
				*i++ = r * sectors + s;
				*i++ = r * sectors + ((s + 1) % sectors);
				*i++ = (r + 1) * sectors + s;
				*i++ = (r + 1) * sectors + s;
				*i++ = (r + 1) * sectors + ((s + 1) % sectors);
				*i++ = r * sectors + ((s + 1) % sectors);
			}
		}
		//cout<<"tmp Nnm = "<<num<<endl;
	}
}

static const GLfloat vertex_positions[] =
{
	-0.25f,  0.25f, -0.25f,
	-0.25f, -0.25f, -0.25f,
	0.25f, -0.25f, -0.25f,

	0.25f, -0.25f, -0.25f,
	0.25f,  0.25f, -0.25f,
	-0.25f,  0.25f, -0.25f,

	0.25f, -0.25f, -0.25f,
	0.25f, -0.25f,  0.25f,
	0.25f,  0.25f, -0.25f,

	0.25f, -0.25f,  0.25f,
	0.25f,  0.25f,  0.25f,
	0.25f,  0.25f, -0.25f,

	0.25f, -0.25f,  0.25f,
	-0.25f, -0.25f,  0.25f,
	0.25f,  0.25f,  0.25f,

	-0.25f, -0.25f,  0.25f,
	-0.25f,  0.25f,  0.25f,
	0.25f,  0.25f,  0.25f,

	-0.25f, -0.25f,  0.25f,
	-0.25f, -0.25f, -0.25f,
	-0.25f,  0.25f,  0.25f,

	-0.25f, -0.25f, -0.25f,
	-0.25f,  0.25f, -0.25f,
	-0.25f,  0.25f,  0.25f,

	-0.25f, -0.25f,  0.25f,
	0.25f, -0.25f,  0.25f,
	0.25f, -0.25f, -0.25f,

	0.25f, -0.25f, -0.25f,
	-0.25f, -0.25f, -0.25f,
	-0.25f, -0.25f,  0.25f,

	-0.25f,  0.25f, -0.25f,
	0.25f,  0.25f, -0.25f,
	0.25f,  0.25f,  0.25f,

	0.25f,  0.25f,  0.25f,
	-0.25f,  0.25f,  0.25f,
	-0.25f,  0.25f, -0.25f

};

static const GLfloat body_positions[] =
{
	-0.5f,  -0.25f, -0.25f,
	-0.5f, -1.75f, -0.25f,
	0.5f, -1.75f, -0.25f,

	0.5f, -1.75f, -0.25f,
	0.5f,  1.75f, -0.25f,
	-0.5f,  -0.25f, -0.25f,

	0.5f, -1.75f, -0.25f,
	0.5f, -1.75f,  0.25f,
	0.5f,  -0.25f, -0.25f,

	0.5f, -1.75f,  0.25f,
	0.5f,  -0.25f,  0.25f,
	0.5f,  -0.25f, -0.25f,

	0.5f, -1.75f,  0.25f,
	-0.5f, -1.75f,  0.25f,
	0.5f,  -0.25f,  0.25f,

	-0.5f, -1.75f,  0.25f,
	-0.5f, -0.25f,  0.25f,
	0.5f,  -0.25f,  0.25f,

	-0.5f, -1.75f,  0.25f,
	-0.5f, -1.75f, -0.25f,
	-0.5f,  -0.25f,  0.25f,

	-0.5f, -1.75f, -0.25f,
	-0.5f,  -0.25f, -0.25f,
	-0.5f, -0.25f,  0.25f,

	-0.5f, -1.75f,  0.25f,
	0.5f, -1.75f,  0.25f,
	0.5f, -1.75f, -0.25f,

	0.5f, -1.75f, -0.25f,
	-0.5f, -1.75f, -0.25f,
	-0.5f, -1.75f,  0.25f,

	-0.5f, -0.25f, -0.25f,
	0.5f,  -0.25f, -0.25f,
	0.5f,  -0.25f,  0.25f,

	0.5f,  -0.25f,  0.25f,
	-0.5f, -0.25f,  0.25f,
	-0.5f, -0.25f, -0.25f

};

GLuint          program;
GLuint          vao;
GLuint          buffer;
GLint           mv_location;
GLint           mv2_location;
GLint           proj_location;
mat4 projection, view;

void My_Init()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	program = glCreateProgram();
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, fs_source, NULL);
	glCompileShader(fs);

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, vs_source, NULL);
	glCompileShader(vs);

	glAttachShader(program, vs);
	glAttachShader(program, fs);

	glLinkProgram(program);

	
	createSphere(1.0f, 10, 10);
	putData();

	//  cout << sphereVertex[16] << endl ;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	glBufferData(GL_ARRAY_BUFFER, indices.size() * 3 * sizeof(GLfloat), sphereVertex, GL_STATIC_DRAW);
	// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	mv_location = glGetUniformLocation(program, "mv_matrix");


	glEnableVertexAttribArray(0);

	// glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);

}

// GLUT callback. Called to draw the scene.
void My_Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	static const GLfloat green[] = { 0.0f, 0.25f, 0.0f, 1.0f };
	static const GLfloat one = 1.0f;

	glClearBufferfv(GL_COLOR, 0, green);
	glClearBufferfv(GL_DEPTH, 0, &one);

	glUseProgram(program);


	mat4 Identy_Init = mat4(1.0f);

	float currentTime = glutGet(GLUT_ELAPSED_TIME) * 0.001f;


	mat4 scalebody = projection * view;
	glUniformMatrix4fv(mv_location, 1, GL_FALSE, &scalebody[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, indices.size() * 3);

	

	glutSwapBuffers();
}

void My_Timer(int val)
{
	glutPostRedisplay();

	if (timer_enable)
		glutTimerFunc(16, My_Timer, val);
}
void My_Menu(int id) {


	switch (id) {
	case 2:
		if (!timer_enable) {
			timer_enable = true;
			glutTimerFunc(16, My_Timer, 0);
		}
		break;
	case 3:
		timer_enable = false;
		break;
	case 4:
		w_hand = 60.0f;
		w_head = 10.0f;
		w_leg = 30.0f;
		break;
	case 5:
		w_hand = 0.0f;
		w_head = 0.0f;
		w_leg = 0.0f;
		break;
	}


}
void My_Reshape(int width, int height)
{
	glViewport(0, 0, width, height);

	float viewportAspect = (float)width / (float)height;

	projection = perspective(radians(60.0f), viewportAspect, 0.1f, 100.0f);
	view = lookAt(vec3(0.0f, 1.5f, 4.0f), vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, 1.0f, 0.0f));

}



int main(int argc, char *argv[])
{
	// Change working directory to source code path
	chdir(__FILEPATH__);
	// Initialize GLUT and GLEW, then create a window.
	////////////////////
	glutInit(&argc, argv);
#ifdef _MSC_VER
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#else
	glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#endif
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(600, 600);
	glutCreateWindow(__FILENAME__); // You cannot use OpenGL functions before this line; The OpenGL context must be created first by glutCreateWindow()!
#ifdef _MSC_VER
	glewInit();
#endif
	dumpInfo();
	My_Init();
	////////////////////

	// Register GLUT callback functions.
	///////////////////////////////

	int menu_main = glutCreateMenu(My_Menu);
	int menu_timer = glutCreateMenu(My_Menu);
	int menu_new = glutCreateMenu(My_Menu);

	glutSetMenu(menu_main);
	glutAddSubMenu("Time", menu_timer);
	glutAddSubMenu("New", menu_new);
	glutAddMenuEntry("EXIT", 1);

	glutSetMenu(menu_timer);
	glutAddMenuEntry("STAR", 2);
	glutAddMenuEntry("STOP", 3);

	glutSetMenu(menu_new);
	glutAddMenuEntry("WLAK", 4);
	glutAddMenuEntry("Stand", 5);

	glutSetMenu(menu_main);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glutDisplayFunc(My_Display);
	glutReshapeFunc(My_Reshape);

	glutTimerFunc(16, My_Timer, 0);

	///////////////////////////////

	// Enter main event loop.
	//////////////
	glutMainLoop();
	//////////////
	return 0;
}
