#include "../../Include/Common.h"

using namespace glm;

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

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_positions), vertex_positions, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	mv_location = glGetUniformLocation(program, "mv_matrix");

	/*
	glBufferData(GL_ARRAY_BUFFER, sizeof(body_positions), body_positions, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	*/
	glEnableVertexAttribArray(0);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);

	/*mv2_location = glGetUniformLocation(program, "mv2_matrix");
	proj_location = glGetUniformLocation(program, "proj_matrix");*/
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


	mat4 Identy_Init= mat4(1.0f);

	float currentTime = glutGet(GLUT_ELAPSED_TIME) * 0.001f;

	//for(int i =0; i < 24; ++i)
	//	{
	//float f = (float)i+(float)currentTime * 0.3f;

	//mv_matrix = translate(mv_matrix,vec3(sinf(2.1f * f) * 2.0f,cosf(1.7f * f) * 2.0f, sinf(1.3f * f) * cosf(1.5f * f) * 2.0f));
	//mv_matrix = rotate(mv_matrix,deg2rad(currentTime * 45.0f),vec3(0.0f, 1.0f, 0.0f));
	//mv_matrix = rotate(mv_matrix,deg2rad(currentTime * 81.0f),vec3(1.0f, 0.0f, 0.0f));

	mat4 mv2_body = translate(Identy_Init, vec3(0.0f, 0.0f, 0.0f));
    mv2_body = rotate(mv2_body, deg2rad(currentTime * 45.0f), vec3(0.0f, 1.0f, 0.0f));
	mv2_body = translate(mv2_body, vec3(0.0f, 0.0f, 0.0f));
	mat4 scalebody = scale(Identy_Init, vec3(2.5f, 3.0f, 1.0f));
	// mv2_body = rotate(mv2_body, deg2rad(currentTime * 45.0f), vec3(0.0f, 1.0f, 0.0f));
	scalebody = projection * view * mv2_body * scalebody;
	glUniformMatrix4fv(mv_location, 1, GL_FALSE, &scalebody[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	

	mat4 mv_head = translate(Identy_Init, vec3(0.0f, 3.0f, -4.0f));
	mat4 scaleHead = scale(Identy_Init, vec3(3.5f, 3.5f, 2.0f));
	scaleHead = projection * view *mv_head*mv2_body *scaleHead;
	glUniformMatrix4fv(mv_location, 1, GL_FALSE, &scaleHead[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	mat4 mv3_leg1 = translate(Identy_Init, vec3(-0.5f, -3.0f, -4.0f));
	mat4 scaleLeg1 = scale(Identy_Init, vec3(1.0f, 4.0f, 1.0f));

	scaleLeg1 = projection * view *mv3_leg1*scaleLeg1*mv2_body;
	glUniformMatrix4fv(mv_location, 1, GL_FALSE, &scaleLeg1[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	mat4 mv4_leg2 = translate(Identy_Init, vec3(0.5f, -3.0f, -4.0f));
	mat4 scalelLeg2 = scale(Identy_Init, vec3(1.0f, 4.0f, 1.0f));

	scalelLeg2 = projection * view *mv4_leg2* scalelLeg2*mv2_body;
	glUniformMatrix4fv(mv_location, 1, GL_FALSE, &scalelLeg2[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);



	mat4 mv5_hand1 = translate(Identy_Init, vec3(1.5f, -0.75f, -4.0f));
	mat4 scaleHand1 = scale(Identy_Init,vec3(1.0f, 4.0f, 1.0f));
	scaleHand1 = projection * view *mv5_hand1*mv2_body*scaleHand1;


	glUniformMatrix4fv(mv_location, 1, GL_FALSE, &scaleHand1[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);


	mat4 scalelHand2 = scale(Identy_Init, vec3(1.0f, 4.0f, 1.0f));
	mat4 mv6_hand2 = translate(Identy_Init, vec3(-1.5f, -0.75f, -4.0f));

	//mat4 Hand2 = rotate(mv6_matrix, deg2rad(-60.0f), vec3(0.0f, 0.0f, 1.0f));
	scalelHand2 = projection * view *mv6_hand2*scalelHand2*mv2_body;


	glUniformMatrix4fv(mv_location, 1, GL_FALSE, &scalelHand2[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	//}
	
	glutSwapBuffers();
}

void My_Reshape(int width, int height)
{
	glViewport(0, 0, width, height);

	float viewportAspect = (float)width / (float)height;

	projection = perspective(radians(60.0f), viewportAspect, 0.1f, 100.0f);
	view = lookAt(vec3(0.0f, 1.5f, 4.0f), vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, 1.0f, 0.0f));
}

void My_Timer(int val)
{
	glutPostRedisplay();
	glutTimerFunc(16, My_Timer, val);
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
