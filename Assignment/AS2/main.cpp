#include "../Externals/Include/Include.h"
#include <math.h>
#define MENU_TIMER_START 1
#define MENU_TIMER_STOP 2
#define MENU_EXIT 3


struct Shape {
	GLuint vao;
	GLuint vbo_position;
	GLuint vbo_normal;
	GLuint vbo_texcoord;
	GLuint ibo;
	int drawCount;
	int materialID;
};
struct Material {
	GLuint diffuse_tex;
};

void loadModel();

GLubyte timer_cnt = 0;
bool timer_enabled = true;
unsigned int timer_speed = 16;

using namespace glm;
using namespace std;

GLuint program;
GLint um4mv, um4p;
GLuint texture_location;

mat4 view;					// V of MVP, viewing matrix
mat4 projection;			// P of MVP, projection matrix
mat4 model;					// M of MVP, model matrix
float X , Y , Z;
float viewportAspect;
mat4 mvp;
vector<Material> v_material;
vector<Shape> v_shape;
int shapeNum;

float mouse_x, mouse_y, t_x, t_y;
int walk = 1;

mat4 scaleOne, M, R_X, R_Y, R_Z, T;
float dis_x = 0.0f;
float dis_z = 0.0f;

vec3 eye, center, up;
char filename[][50] = { "sponza.obj", "two/sponza.obj"};
int filesize = 2;
int now = 0;

float w_now = 0, w_dis = 0, s_now = 0, s_dis = 0;
float a_now = 0, a_dis = 0, d_now = 0, d_dis = 0;

int ma_tmp = 0, sha_tmp = 0;

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
	delete[] srcp[0];
	delete[] srcp;
}
void shaderLog(GLuint shader)
{
	GLint isCompiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		GLchar* errorLog = new GLchar[maxLength];
		glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

		printf("%s\n", errorLog);
		delete errorLog;
	}
}

// define a simple data structure for storing texture image raw data
typedef struct _TextureData
{
	_TextureData(void) :
		width(0),
		height(0),
		data(0)
	{
	}

	int width;
	int height;
	unsigned char* data;
} TextureData;

// load a png image and return a TextureData structure with raw data
// not limited to png format. works with any image format that is RGBA-32bit
TextureData loadPNG(const char* const pngFilepath)
{
	TextureData texture;
	int components;

	// load the texture with stb image, force RGBA (4 components required)
	stbi_uc *data = stbi_load(pngFilepath, &texture.width, &texture.height, &components, 4);

	// is the image successfully loaded?
	if (data != NULL)
	{
		// copy the raw data
		size_t dataSize = texture.width * texture.height * 4 * sizeof(unsigned char);
		texture.data = new unsigned char[dataSize];
		memcpy(texture.data, data, dataSize);

		// mirror the image vertically to comply with OpenGL convention
		for (size_t i = 0; i < texture.width; ++i)
		{
			for (size_t j = 0; j < texture.height / 2; ++j)
			{
				for (size_t k = 0; k < 4; ++k)
				{
					size_t coord1 = (j * texture.width + i) * 4 + k;
					size_t coord2 = ((texture.height - j - 1) * texture.width + i) * 4 + k;
					std::swap(texture.data[coord1], texture.data[coord2]);
				}
			}
		}

		// release the loaded image
		stbi_image_free(data);
	}

	return texture;
}

void loadModel()
{
	const aiScene *scene = aiImportFile(filename[now], aiProcessPreset_TargetRealtime_MaxQuality);
	cout << "scene material num = " << scene->mNumMaterials << endl;
	cout << "scene mesh num = " << scene->mNumMeshes << endl;
	for (unsigned int i = 0; i < scene->mNumMaterials; ++i) {
		aiMaterial *material = scene->mMaterials[i];
		Material mmaterial;
		aiString texturePath;
		if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == aiReturn_SUCCESS) {
			// load width, height and data from texturePath.C_Str();
			TextureData tex = loadPNG(texturePath.C_Str());

			cout << texturePath.C_Str() << " " << tex.width << " " << tex.height << endl;
			glGenTextures(1, &mmaterial.diffuse_tex);
			glBindTexture(GL_TEXTURE_2D, mmaterial.diffuse_tex);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tex.width, tex.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex.data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else {
			scene->mMaterials[1]->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath);
			TextureData tex = loadPNG(texturePath.C_Str());

			glGenTextures(1, &mmaterial.diffuse_tex);
			glBindTexture(GL_TEXTURE_2D, mmaterial.diffuse_tex);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tex.width, tex.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex.data);
			glGenerateMipmap(GL_TEXTURE_2D);
		} // save material

		v_material.push_back(mmaterial);
	}

	shapeNum = scene->mNumMeshes;
	for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
		aiMesh *mesh = scene->mMeshes[i];
		Shape shape;
		glGenVertexArrays(1, &shape.vao);
		glBindVertexArray(shape.vao);


		glGenBuffers(1, &shape.vbo_position);
		glGenBuffers(1, &shape.vbo_normal);
		glGenBuffers(1, &shape.vbo_texcoord);
		glGenBuffers(1, &shape.ibo);
		// create 3 vbos to hold data 
		float *normal = new float[mesh->mNumVertices * 3];
		float *position = new float[mesh->mNumVertices * 3];
		float *texposition = new float[mesh->mNumVertices * 2];
		// mesh->mVertices[v][0~2] => position 
		// mesh->mNormals[v][0~2] => normal 
		// mesh->mTextureCoords[0][v][0~1] => texcoord
		unsigned int x = 0;
		unsigned int y = 0;
		for (unsigned int v = 0; v < mesh->mNumVertices; ++v)
		{
			position[x] = mesh->mVertices[v][0];
			position[x + 1] = mesh->mVertices[v][1];
			position[x + 2] = mesh->mVertices[v][2];
			normal[x] = mesh->mNormals[v][0];
			normal[x + 1] = mesh->mNormals[v][1];
			normal[x + 2] = mesh->mNormals[v][2];
			texposition[y] = mesh->mTextureCoords[0][v][0];
			texposition[y + 1] = mesh->mTextureCoords[0][v][1];
			y += 2;
			x += 3;
		}

		glBindBuffer(GL_ARRAY_BUFFER, shape.vbo_position);
		glBufferData(GL_ARRAY_BUFFER, mesh->mNumVertices * sizeof(float) * 3, position, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

		glBindBuffer(GL_ARRAY_BUFFER, shape.vbo_normal);
		glBufferData(GL_ARRAY_BUFFER, mesh->mNumVertices * sizeof(float) * 3, normal, GL_STATIC_DRAW);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);

		glBindBuffer(GL_ARRAY_BUFFER, shape.vbo_texcoord);
		glBufferData(GL_ARRAY_BUFFER, mesh->mNumVertices * sizeof(float) * 2, texposition, GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

		// create 1 ibo to hold data
		unsigned int *index = new unsigned int[mesh->mNumFaces * sizeof(unsigned int) * 3];
		int faceIndex = 0;
		for (unsigned int f = 0; f < mesh->mNumFaces; ++f) {

			const struct aiFace* face = &mesh->mFaces[f];
			std::memcpy(&index[faceIndex], face->mIndices, sizeof(unsigned int) * 3);
			faceIndex += 3;
		}
		glBindBuffer(GL_ARRAY_BUFFER, shape.ibo);
		glBufferData(GL_ARRAY_BUFFER, mesh->mNumFaces * sizeof(unsigned int) * 3, index, GL_STATIC_DRAW);


		shape.materialID = mesh->mMaterialIndex;
		shape.drawCount = mesh->mNumFaces * 3;

		v_shape.push_back(shape);
	}

	aiReleaseImport(scene);

	printf("@@@@::: %d %d\n", v_material.size(), v_shape.size());
}

void My_Init()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
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

	texture_location = glGetUniformLocation(program, "tex");
	um4mv = glGetUniformLocation(program, "um4mv");
	um4p = glGetUniformLocation(program, "um4p");

	glUseProgram(program);

	loadModel();
}

void My_Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	w_now = w_dis - w_now;
	s_now = s_dis - s_now;
	d_now = d_dis - d_now;
	a_now = a_dis - a_now;
	
	vec3 forward = eye - center;
	vec3 nforward = normalize(forward);
	vec3 right = cross(forward, up);
	vec3 nright = normalize(right);
	vec3 up_vec = cross(right, forward);

	eye = eye + (vec3(s_now) - vec3(w_now)) * nforward + (vec3(a_now) - vec3(d_now))* nright;
	center = center + (vec3(s_now) - vec3(w_now)) * nforward + (vec3(a_now) - vec3(d_now))* nright;
	

	w_now = w_dis;
	s_now = s_dis;
	d_now = d_dis;
	a_now = a_dis;
	
	view = lookAt(eye, center, up_vec);
	
	scaleOne = mat4(1.0f);
	M = scaleOne;
	mvp = projection * view * M;

	GLuint texture_location = glGetUniformLocation(program, "tex");
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(texture_location, 0);
	glUniformMatrix4fv(um4mv, 1, GL_FALSE, value_ptr(mvp));
	glUniformMatrix4fv(um4p, 1, GL_FALSE, value_ptr(M));
	glUniform1i(texture_location, 0);

	
	for (int i = sha_tmp; i < v_shape.size(); i++)
	{
		glBindVertexArray(v_shape[i].vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, v_shape[i].ibo);

		glUniformMatrix4fv(um4mv, 1, GL_FALSE, value_ptr(mvp));
		glUniformMatrix4fv(um4p, 1, GL_FALSE, value_ptr(M));

		glBindTexture(GL_TEXTURE_2D, v_material[v_shape[i].materialID].diffuse_tex);
		glDrawElements(GL_TRIANGLES, v_shape[i].drawCount, GL_UNSIGNED_INT, 0);
	}
	glutSwapBuffers();
}

void My_Reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	viewportAspect = (float)width / (float)height;
	X = 0.0f;
	Y = 0.0f;
	Z = 0.0f;

	projection = perspective(radians(45.0f), viewportAspect, 0.1f, 3000.f);
	
	if (now == 1) {
		eye = vec3(0.0f, 3.0f, 0.0f);
		center = vec3(2.0f, 3.0f, 0.0f);
		up = vec3(0.0f, 1.0f, 0.0f);
	}
	else if (now == 0) {
		eye = vec3(-20.0f, 50.0f, 0.0f);
		center = vec3(30.0f, 50.0f, 0.0f);
		up = vec3(0.0f, 1.0f, 0.0f);
	}
	view = lookAt(eye, center, up);
}

void My_Timer(int val)
{
	glutPostRedisplay();
	glutTimerFunc(timer_speed, My_Timer, val);
}

void My_Mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)
	{
		printf("Mouse %d is pressed at (%d, %d)\n", button, x, y);
		mouse_x = (float)x; 
		mouse_y = (float)y;
	}
	else if (state == GLUT_UP)
	{
		printf("Mouse %d is released at (%d, %d)\n", button, x, y);
	}
}

void onMouseMotion(int x, int y) {
	//printf("%18s(): (%d, %d) mouse move\n", __FUNCTION__, x, y);
	float dis = sqrt(pow(eye.x - center.x, 2) + pow(eye.z - center.z,2));
	t_x += ((float)x - mouse_x) / 8000.0f;
	t_y = (mouse_y - (float)y) / 800.0f;
	center.z = eye.z + dis * sin(t_x);
	center.x = eye.x + dis * cos(t_x);
	//printf("@@ %f @@", t_y);
	center.y += t_y;
	//printf("==== %f, %f, %f ===\n", center.x, center.y, center.z);
}

void My_Keyboard(unsigned char key, int x, int y)
{
	if (key == 'w' || key == 'W') {
		if(now == 0)	w_dis += 40.0f;
		else {
			w_dis += 0.1f;
		}
		walk = 1;
	}
	if (key == 'a' || key == 'A') {
		if (now == 0)	a_dis += 40.0f;
		else {
			a_dis += 0.1f;
		}
		walk = 1;
	}
	if (key == 's' || key == 'S') {
		if (now == 0)	s_dis += 40.0f;
		else {
			s_dis += 0.1f;
		}
	}
	if (key == 'd' || key == 'D') {
		if (now == 0)	d_dis += 40.0f;
		else {
			d_dis += 0.1f;
		}
	}
	if (key == 'z' || key == 'Z') {
		if (now == 0) eye.y += 5.0f;
		else  eye.y += 0.4f;
	}
	if (key == 'x' || key == 'X') {
		if(now == 0) eye.y -= 5.0f;
		else eye.y -= 0.4f;
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
		now++;
		if (now >= filesize) now = 0;
		ma_tmp = v_material.size();
		sha_tmp = v_shape.size();

		My_Init();

		X = 0.0f;
		Y = 0.0f;
		Z = 0.0f;
		if (now == 1) {
			eye = vec3(0.0f, 3.0f, 0.0f);
			center = vec3(2.0f, 3.0f, 0.0f);
			up = vec3(0.0f, 1.0f, 0.0f);
		}
		else if (now == 0) {
			eye = vec3(-4.0f, 20.0f, 0.0f);
			center = vec3(0.0f, 20.0f, 0.0f);
			up = vec3(0.0f, 1.0f, 0.0f);
		}
		view = lookAt(eye, center, up);
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


int main(int argc, char *argv[])
{
#ifdef __APPLE__
	// Change working directory to source code path
	chdir(__FILEPATH__("/../Assets/"));
#endif
	// Initialize GLUT and GLEW, then create a window.
	glutInit(&argc, argv);
#ifdef _MSC_VER
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#else
	glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#endif
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(600, 600);
	glutCreateWindow("AS2_Framework"); // You cannot use OpenGL functions before this line; The OpenGL context must be created first by glutCreateWindow()!
#ifdef _MSC_VER
	glewInit();
#endif
	glPrintContextInfo();
	My_Init();

	// Create a menu and bind it to mouse right button.
	int menu_main = glutCreateMenu(My_Menu);
	int menu_timer = glutCreateMenu(My_Menu);

	glutSetMenu(menu_main);
	glutAddSubMenu("Timer", menu_timer);
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
	glutMotionFunc(onMouseMotion);
	glutKeyboardFunc(My_Keyboard);
	glutSpecialFunc(My_SpecialKeys);
	glutTimerFunc(timer_speed, My_Timer, 0);

	// Enter main event loop.
	glutMainLoop();

	return 0;
}
