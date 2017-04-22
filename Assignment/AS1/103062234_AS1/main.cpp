#include "../Include/Common.h"


//For GLUT to handle 
#define MENU_TIMER_START 1
#define MENU_TIMER_STOP 2
#define MENU_WALK_ON 3
#define MENU_WALK_OFF 4
#define MENU_DANCE 5
#define MENU_EXIT 6

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923
#endif

using namespace glm;
using namespace std;

//GLUT timer variable
GLubyte timer_cnt = 0;
bool timer_enabled = true;
unsigned int timer_speed = 16;
GLfloat *cylinder_positions;
GLfloat *sphere_positions;
static const GLfloat cube_positions[] =
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

std::vector<GLfloat> vertices;
std::vector<GLushort> indices;

float a = 0.0f;
float z = 0.0f;
float b = 0.0f;
//GLuint buffer;
GLuint vao[3], vbo[3];
GLint mvLoc, colLoc;

mat4 view;					// V of MVP, viewing matrix
mat4 projection;			// P of MVP, projection matrix
mat4 model;					// M of MVP, model matrix
mat4 mvp;

mat4 scaleOne;

mat4 scaleBody, rotateBody, transBody, mBody;

mat4 scaleHead, rotateHead, transHead, mHead;
mat4 scaleNeck, rotateNeck, transNeck, mNeck;

mat4 scaleLArm1, rotateLArm1, transLArm1, mLArm1;
mat4 scaleRArm1, rotateRArm1, transRArm1, mRArm1;
mat4 scaleLArm2, rotateLArm2, transLArm2, mLArm2;
mat4 scaleRArm2, rotateRArm2, transRArm2, mRArm2;
mat4 scaleLHand, rotateLHand, transLHand, mLHand;
mat4 scaleRHand, rotateRHand, transRHand, mRHand;

mat4 scaleLLeg1, rotateLLeg1, transLLeg1, mLLeg1;
mat4 scaleRLeg1, rotateRLeg1, transRLeg1, mRLeg1;
mat4 scaleLLeg2, rotateLLeg2, transLLeg2, mLLeg2;
mat4 scaleRLeg2, rotateRLeg2, transRLeg2, mRLeg2;
mat4 scaleLFoot, rotateLFoot, transLFoot, mLFoot;
mat4 scaleRFoot, rotateRFoot, transRFoot, mRFoot;

mat4 scaleSphere, rotateSphere, transSphere, mSphere;

GLuint program;			// shader program id
GLuint tex;

vec3 temp = vec3();
float degree_body = 0.0f;
float degree_hand = 0.0f;
float degree_hand2 = 0.0f;
float degree_foot = 0.0f;
int down = 0;
int walk = 0;
int dance = 0;
int mode = 0;

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

void putData()
{
	printf("%d !!!!!!!\n", indices.size());
	sphere_positions = new GLfloat[indices.size() * 3];
	int i = 0;
	vector<GLushort>::iterator tmp;
	for (tmp = indices.begin(); tmp != indices.end(); tmp++) {
		//cout<<*tmp<<endl;
		sphere_positions[i] = vertices[(*tmp) * 3];
		sphere_positions[i + 1] = vertices[(*tmp) * 3 + 1];
		sphere_positions[i + 2] = vertices[(*tmp) * 3 + 2];
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
			float const y = sin(- M_PI_2 + M_PI * r * R);
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
	cout << "NUM = " << num << endl;
	cout << "indices num = " << indices.size() << endl;

}

void drawCylinder(int sides, float r, float h) 
{
	cylinder_positions = new float[4*sides];
	float angle = 360 / sides;
	float halfHeight = h / 2;
	int now = 0;
	float prev_x, prev_y;
	float x, y;
	for (int i = 0; i < sides; i++) {
		if (i == 0) {
			prev_x = cos(radians(i * angle)) * r;
			prev_y = sin(radians(i * angle)) * r;
			cylinder_positions[now++] = prev_x;
			cylinder_positions[now++] = prev_y;
			cylinder_positions[now++] = halfHeight;
			cylinder_positions[now++] = prev_x;
			cylinder_positions[now++] = prev_y;
			cylinder_positions[now++] = -halfHeight;
		}
		else {
			prev_x = cos(radians((i-1) * angle)) * r;
			prev_y = sin(radians((i-1) * angle)) * r;
			if (i != sides - 1) {
				x = cos(radians(i * angle)) * r;
				y = sin(radians(i * angle)) * r;
			}
			else {
				x = cos(radians(0 * angle)) * r;
				y = sin(radians(0 * angle)) * r;
			}
			
			cylinder_positions[now++] = x;
			cylinder_positions[now++] = y;
			cylinder_positions[now++] = halfHeight;

			cylinder_positions[now++] = prev_x;
			cylinder_positions[now++] = prev_y;
			cylinder_positions[now++] = -halfHeight;
			cylinder_positions[now++] = x;
			cylinder_positions[now++] = y;
			cylinder_positions[now++] = halfHeight;
			cylinder_positions[now++] = x;
			cylinder_positions[now++] = y;
			cylinder_positions[now++] = -halfHeight;

			cylinder_positions[now++] = prev_x;
			cylinder_positions[now++] = prev_y;
			cylinder_positions[now++] = halfHeight;
			cylinder_positions[now++] = x;
			cylinder_positions[now++] = y;
			cylinder_positions[now++] = halfHeight;
			cylinder_positions[now++] = 0.0f;
			cylinder_positions[now++] = 0.0f;
			cylinder_positions[now++] = halfHeight;

			cylinder_positions[now++] = prev_x;
			cylinder_positions[now++] = prev_y;
			cylinder_positions[now++] = -halfHeight;
			cylinder_positions[now++] = x;
			cylinder_positions[now++] = y;
			cylinder_positions[now++] = -halfHeight;
			cylinder_positions[now++] = 0.0f;
			cylinder_positions[now++] = 0.0f;
			cylinder_positions[now++] = -halfHeight;

			if (i != sides - 1) {
				cylinder_positions[now++] = x;
				cylinder_positions[now++] = y;
				cylinder_positions[now++] = -halfHeight;
				cylinder_positions[now++] = x;
				cylinder_positions[now++] = y;
				cylinder_positions[now++] = -halfHeight;
			}
			
		}

	}

}

void showPosition(GLfloat * test) 
{
	int i;
	//printf("%d !!!!!!!\n", sizeof(test));
	for (i = 0; i < indices.size()*3; i+=3) {
		printf("%f   %f   %f\n", test[i], test[i + 1], test[i + 2]);
	}
}

void generate_texture(float*data, int width, int height)
{
	int x, y;
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			data[(y * width + x) * 4 + 0] = (float)((x & y) & 0xFF) / 255.0f;
			data[(y * width + x) * 4 + 1] = (float)((x | y) & 0xFF) / 255.0f;
			data[(y * width + x) * 4 + 2] = (float)((x ^ y) & 0xFF) / 255.0f;
			data[(y * width + x) * 4 + 3] = 1.0f;
		}
	}
}

// OpenGL initialization
void My_Init()
{
	createSphere(0.5f, 50, 50);
	putData();

	int sides = 8;
	//drawCylinder(sides, 1.0f, 2.0f);
	 
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

	// Tell OpenGL to use this shader program now
	glUseProgram(program);

	glGenVertexArrays(3, vao); // We can also generate multiple VAOs or buffers at the same time
	glGenBuffers(3, vbo);

	//Enable shader layout location 0 and 1 for vertex and color   
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	// ================================
	// CUBE setup
	// ===============================
	glBindVertexArray(vao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_positions), cube_positions, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	// ================================
	// SPHERE setup
	// ===============================
	glBindVertexArray(vao[1]);	// Note that we bind to a different VAO now
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);	// And a different VBO
	glBufferData(GL_ARRAY_BUFFER, indices.size() * 3*sizeof(GLfloat), sphere_positions, GL_STATIC_DRAW);
	printf("%d %d\n", sizeof(indices), sizeof(sphere_positions));
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
	
	// ================================
	// CYLINDER setup
	// ===============================
	/*glBindVertexArray(vao[2]);	// Note that we bind to a different VAO now
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);	// And a different VBO
	glBufferData(GL_ARRAY_BUFFER, sides * 4 * sizeof(GLfloat), cylinder_positions, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);*/

	//glEnable(GL_CULL_FACE);
	//glFrontFace(GL_CW);

	mvLoc = glGetUniformLocation(program, "mv_matrix");
	colLoc = glGetUniformLocation(program, "col_matrix");

	glEnable(GL_TEXTURE_2D);       // Enable Texture Mapping
}

// GLUT callback. Called to draw the scene.
void My_Display()
{
	static const GLfloat background[] = { 0.5f, 0.5f, 0.8f, 1.0f };
	static const GLfloat one = 1.0f;
	glClearBufferfv(GL_COLOR, 0, background);
	glClearBufferfv(GL_DEPTH, 0, &one);

	glUseProgram(program);

	float currentTime = glutGet(GLUT_ELAPSED_TIME) * 0.001f;
	//printf("%f\n",currentTime);

	scaleOne = mat4(1.0f);

	// Animation
	if (mode == 1) {
		a -= 0.5f;
		if (a < 0.1f) mode = 0;
	}
	else if (mode == 2) {
		a += 0.5f;
		if (a > -0.1f) mode = 0;
	}
	else {
		if (walk == 1) {
			dance = 0;
			degree_foot = deg2rad(sin(currentTime*4.0f)*10.0f);
			//degree_hand = deg2rad(sin(currentTime*5.0f)*45.0f);
		}
		else {
			if (dance != 0) {
				dance++;
				if (dance <= 50) {
					degree_hand += deg2rad(-1.0f);
					degree_foot += deg2rad(-0.5f);
				}
				else {
					if (dance < 232) degree_body += deg2rad(4.0f);
					else if (dance < 282) {
						degree_hand += deg2rad(1.0f);
						degree_foot += deg2rad(0.5f);
					}
					else dance = 0;
				}
				if (dance != 0)	dance++;
			}
		}
	}

	

	// CUBE
	glBindVertexArray(vao[0]);

	// Body
	scaleBody = scale(scaleOne, vec3(1.4f, 3.0f, 0.8f));
	rotateBody = rotate(scaleOne, degree_body, vec3(0.0f, 1.0f, 0.0f));
	transBody = translate(rotateBody, vec3(0.0f, 0.0f, 0.0f+a));
	mBody = transBody * scaleBody;
	mvp = projection * view * mBody;
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, value_ptr(mvp));
	glUniform1i(colLoc, 3);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	
	// Neck
	scaleNeck = scale(scaleOne, vec3(0.2f, 0.2f, 0.2f));
	rotateNeck = rotate(transBody, 0.0f, vec3(0.0f, 1.0f, 0.0f));
	transNeck = translate(rotateNeck, vec3(0.0f, 0.75f, 0.0f));
	mNeck = transNeck * scaleNeck;
	mvp = projection * view * mNeck;
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, value_ptr(mvp));
	glUniform1i(colLoc, 1);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// SPHERE
	glBindVertexArray(vao[1]);

	// Head
	scaleHead = scale(scaleOne, vec3(0.6f, 0.6f, 0.6f));
	rotateHead = rotate(transNeck, 0.0f, vec3(0.0f, 1.0f, 0.0f));
	transHead = translate(rotateHead, vec3(0.0f, 0.35f, 0.0f));
	mHead = transHead * scaleHead;
	mvp = projection * view *  mHead;
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, value_ptr(mvp));
	glUniform1i(colLoc, 0);
	glDrawArrays(GL_TRIANGLES, 0, indices.size()*3);
	
	// Left Arm 1
	scaleLArm1 = scale(scaleOne, vec3(0.8f, 0.2f, 0.2f));
	rotateLArm1 = rotate(transBody, degree_hand, vec3(0.0f, 0.0f, 1.0f));
	transLArm1 = translate(rotateLArm1, vec3(-0.5f, 0.3f, 0.0f));
	mLArm1 = transLArm1 * scaleLArm1;
	mvp = projection * view *  mLArm1;
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, value_ptr(mvp));
	glUniform1i(colLoc, 1);
	glDrawArrays(GL_TRIANGLES, 0, indices.size() * 3);

	// Right Arm 1
	scaleRArm1 = scale(scaleOne, vec3(0.8f, 0.2f, 0.2f));
	rotateRArm1 = rotate(transBody, (-1)*degree_hand, vec3(0.0f, 0.0f, 1.0f));
	transRArm1 = translate(rotateRArm1, vec3(0.5f, 0.3f, 0.0f));
	mRArm1 = transRArm1 * scaleRArm1;
	mvp = projection * view *  mRArm1;
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, value_ptr(mvp));
	glUniform1i(colLoc, 1);
	glDrawArrays(GL_TRIANGLES, 0, indices.size() * 3);

	// Left Arm 2
	scaleLArm2 = scale(scaleOne, vec3(0.2f, 0.8f, 0.2f));
	rotateLArm2 = rotate(transLArm1, 0.0f, vec3(0.0f, 0.0f, 1.0f));
	transLArm2 = translate(rotateLArm2, vec3(-0.4f, 0.35f, 0.0f));
	mLArm2 = transLArm2 * scaleLArm2;
	mvp = projection * view *  mLArm2;
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, value_ptr(mvp));
	glUniform1i(colLoc, 2);
	glDrawArrays(GL_TRIANGLES, 0, indices.size() * 3);

	// Right Arm 2
	scaleRArm2 = scale(scaleOne, vec3(0.2f, 0.8f, 0.2f));
	rotateRArm2 = rotate(transRArm1, 0.0f, vec3(0.0f, 0.0f, 1.0f));
	transRArm2 = translate(rotateRArm2, vec3(0.4f, 0.35f, 0.0f));
	mRArm2 = transRArm2 * scaleRArm2;
	mvp = projection * view *  mRArm2;
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, value_ptr(mvp));
	glUniform1i(colLoc, 2);
	glDrawArrays(GL_TRIANGLES, 0, indices.size() * 3);

	// Left Hand
	scaleLHand = scale(scaleOne, vec3(0.2f, 0.2f, 0.2f));
	rotateLHand = rotate(transLArm2, 0.0f, vec3(1.0f, 0.0f, 0.0f));
	transLHand = translate(rotateLHand, vec3(0.0f, 0.5f, 0.0f));
	mLHand = transLHand * scaleLHand;
	mvp = projection * view *  mLHand;
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, value_ptr(mvp));
	glUniform1i(colLoc, 3);
	glDrawArrays(GL_TRIANGLES, 0, indices.size() * 3);

	// Right Hand
	scaleRHand = scale(scaleOne, vec3(0.2, 0.2f, 0.2f));
	rotateRHand = rotate(transRArm2, 0.0f, vec3(1.0f, 0.0f, 0.0f));
	transRHand = translate(rotateRHand, vec3(0.0f, 0.5f, 0.0f));
	mRHand = transRHand * scaleRHand;
	mvp = projection * view *  mRHand;
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, value_ptr(mvp));
	glUniform1i(colLoc, 3);
	glDrawArrays(GL_TRIANGLES, 0, indices.size() * 3);
	
	
	// Left Leg 1
	scaleLLeg1 = scale(scaleOne, vec3(0.2f, 0.8f, 0.2f));
	if(walk == 1) rotateLLeg1 = rotate(transBody, degree_foot, vec3(1.0f, 0.0f, 0.0f));
	else rotateLLeg1 = rotate(transBody, degree_foot, vec3(0.0f, 0.0f, 1.0f));
	transLLeg1 = translate(rotateLLeg1, vec3(-0.2f, -1.1f, 0.0f));
	mLLeg1 = transLLeg1 * scaleLLeg1;
	mvp = projection * view *  mLLeg1;
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, value_ptr(mvp));
	glUniform1i(colLoc, 1);
	glDrawArrays(GL_TRIANGLES, 0, indices.size() * 3);

	// Right Leg 1
	scaleRLeg1 = scale(scaleOne, vec3(0.2f, 0.8f, 0.2f));
	if(walk == 1) rotateRLeg1 = rotate(transBody, (-1)*degree_foot, vec3(1.0f, 0.0f, 0.0f));
	else rotateRLeg1 = rotate(transBody, 0.0f, vec3(0.0f, 1.0f, 0.0f));
	transRLeg1 = translate(rotateRLeg1, vec3(0.2f, -1.1f, 0.0f));
	mRLeg1 = transRLeg1 * scaleRLeg1;
	mvp = projection * view *  mRLeg1;
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, value_ptr(mvp));
	glUniform1i(colLoc, 1);
	glDrawArrays(GL_TRIANGLES, 0, indices.size() * 3);
	
	// Left Leg 2
	scaleLLeg2 = scale(scaleOne, vec3(0.2f, 0.8f, 0.2f));
	rotateLLeg2 = rotate(transLLeg1, 0.0f, vec3(0.0f, 0.0f, 1.0f));
	transLLeg2 = translate(rotateLLeg2, vec3(0.0f, -0.8f, 0.0f));
	mLLeg2 = transLLeg2 * scaleLLeg2;
	mvp = projection * view *  mLLeg2;
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, value_ptr(mvp));
	glUniform1i(colLoc, 2);
	glDrawArrays(GL_TRIANGLES, 0, indices.size() * 3);

	// Right Leg 2
	scaleRLeg2 = scale(scaleOne, vec3(0.2f, 0.8f, 0.2f));
	rotateRLeg2 = rotate(transRLeg1, 0.0f, vec3(0.0f, 1.0f, 0.0f));
	transRLeg2 = translate(rotateRLeg2, vec3(0.0f, -0.8f, 0.0f));
	mRLeg2 = transRLeg2 * scaleRLeg2;
	mvp = projection * view *  mRLeg2;
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, value_ptr(mvp));
	glUniform1i(colLoc, 2);
	glDrawArrays(GL_TRIANGLES, 0, indices.size() * 3);

	
	// Left Foot
	scaleLFoot = scale(scaleOne, vec3(0.2f, 0.2f, 0.2f));
	rotateLFoot = rotate(transLLeg2, 0.0f, vec3(0.0f, 1.0f, 0.0f));
	transLFoot = translate(rotateLFoot, vec3(0.0f, -0.5f, 0.0f));
	mLFoot = transLFoot * scaleLFoot;
	mvp = projection * view *  mLFoot;
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, value_ptr(mvp));
	glUniform1i(colLoc, 3);
	glDrawArrays(GL_TRIANGLES, 0, indices.size() * 3);

	// Right Foot
	scaleRFoot = scale(scaleOne, vec3(0.2f, 0.2f, 0.2f));
	rotateRFoot = rotate(transRLeg2, 0.0f, vec3(0.0f, 1.0f, 0.0f));
	transRFoot = translate(rotateRFoot, vec3(0.0f, -0.5f, 0.0f));
	mRFoot = transRFoot * scaleRFoot;
	mvp = projection * view *  mRFoot;
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, value_ptr(mvp));
	glUniform1i(colLoc, 3);
	glDrawArrays(GL_TRIANGLES, 0, indices.size() * 3);
	

	//CYLINDER
	/*glBindVertexArray(vao[2]);
	scaleSphere = scale(scaleOne, vec3(2.0f, 2.0f, 2.0f));
	rotateSphere = rotate(transBody, 0.0f, vec3(0.0f, 1.0f, 0.0f));
	transSphere = translate(rotateSphere, vec3(0.0f, -0.3f, -0.5f));
	mSphere = transSphere * scaleSphere;
	mvp = projection * view * mSphere;
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, value_ptr(mvp));
	glUniform1i(colLoc, 2);
	glDrawArrays(GL_TRIANGLES, 0, 32);*/


	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glutSwapBuffers();
}

// Setting up viewing matrix
// This function will be called whenever the size of windows is changing
void My_Reshape(int width, int height)
{
	// Setting where to display and size of OpenGL display area
	glViewport(0, 0, width, height);

	float viewportAspect = (float)width / (float)height;
	projection = perspective(radians(60.0f), viewportAspect, 0.1f, 100.0f);
	view = lookAt(vec3(0.0f, -0.5f, 7.0f), vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, 1.0f, 0.0f));
	
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
	if (key == 'w')
	{
		if (walk == 1) a += 0.5f;
	}
	else if (key == 's')
	{
		if (walk == 1) a -= 0.5f;
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
	case MENU_WALK_ON:
		walk = 1;
		break;
	case MENU_WALK_OFF:
		if (a > 0.0f) mode = 1;
		else if (a < 0.0f) mode = 2;
		walk = 0;
		degree_foot = 0.0f;
		break;
	case MENU_DANCE:
		dance = 1;
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
			printf("Left Mouse %d is pressed at (%d, %d)\n", button, x, y);
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
	glutCreateWindow("103062234_AS1");


#ifdef _MSC_VER
	glewInit();
#endif
	dumpInfo();
	My_Init();

	int menu_main = glutCreateMenu(My_Menu);
	int menu_timer = glutCreateMenu(My_Menu);
	int menu_walk = glutCreateMenu(My_Menu);

	glutSetMenu(menu_main);
	glutAddSubMenu("Timer", menu_timer);
	glutAddSubMenu("Walk", menu_walk);
	glutAddMenuEntry("Dance", MENU_DANCE);
	glutAddMenuEntry("Exit", MENU_EXIT);

	glutSetMenu(menu_timer);
	glutAddMenuEntry("Start", MENU_TIMER_START);
	glutAddMenuEntry("Stop", MENU_TIMER_STOP);

	glutSetMenu(menu_walk);
	glutAddMenuEntry("On", MENU_WALK_ON);
	glutAddMenuEntry("Off", MENU_WALK_OFF);

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