#include <GL\glew.h>
#include <iostream>
#include <Qt3DInput/qkeyevent.h>
#include <QtCore/qelapsedtimer.h>
#include <stdlib.h> 
#include <fstream>
#include <gtx\perpendicular.hpp>
#include <QtCore\qtimer.h>
#include "MyGLWindow.h"

using glm::vec2;

int numTris = 0;
vec2 translate;
vec2 staticPos;
float color[3];
float arenaColor[3];
bool endGame = false;
vec2 velocity;
int oldTime;
QElapsedTimer elapsedTimer;
GLuint programID;
// Create verts
GLfloat Arena[] =
{
	+0.0f, +1.0f, 0.0,
	+1.0f, +0.0f, +0.0f,
	-1.0f, +0.0f, 0.0,
	+1.0f, +0.0f, +0.0f,
	+0.0f, -1.0f, 0.0,
	+1.0f, +0.0f, +0.0f,
	+1.0f, +0.0f, 0.0,
	+1.0f, +0.0f, +0.0f,
};

MyGLWindow::MyGLWindow()
{
	elapsedTimer = QElapsedTimer();
	elapsedTimer.start();
	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(update()));
	timer->start(15);

	setWindowTitle(tr("Move Triangles"));
}


#pragma region OpenGL

void MyGLWindow::sendDataToOpenGL()
{
	const float RED_TRIANGLE_Z = 0.5f;
	const float BLUE_TRIANBLE_Z = -0.5f;

	GLfloat Cross[] =
	{
		-0.005f, +0.015f, RED_TRIANGLE_Z,
		+1.0f, +0.0f, +0.0f,

		+0.005f, +0.015f, RED_TRIANGLE_Z,
		+1.0f, +0.0f, +0.0f,

		-0.005f, -0.015f, RED_TRIANGLE_Z,
		+1.0f, +0.0f, +0.0f,

		+0.005f, -0.015f, RED_TRIANGLE_Z,
		+1.0f, +0.0f, +0.0f,

		-0.015f, +0.005f, RED_TRIANGLE_Z,
		+1.0f, +0.0f, +0.0f,

		-0.015f, -0.005f, RED_TRIANGLE_Z,
		+1.0f, +0.0f, +0.0f,

		+0.015f, +0.005f, RED_TRIANGLE_Z,
		+1.0f, +0.0f, +0.0f,

		+0.015f, -0.005f, RED_TRIANGLE_Z,
		+1.0f, +0.0f, +0.0f
	};

	// Create a BufferID
	GLuint vertexBufferID;
	// Create a Buffer Object with myID
	glGenBuffers(1, &vertexBufferID);
	// 必须要选一个Buffer类型绑定，ELEMENT_ARRAY OR ARRAY
	//（或许可以理解成Buffer object连接上ARRAY）
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	// Send Data to Buffer(哪个buff，data大小，data, 送到哪里）
	glBufferData(GL_ARRAY_BUFFER,10000, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Arena), &Arena);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Arena), sizeof(Cross), &Cross);
	// Enable Vertex Attribute (vertex有很多不同属性，位置是一个);
	glEnableVertexAttribArray(0);
	// 解释Vertex数据（位置，两个数据一个点，GLFloat类型，禁用Normalize，之后解释，之后解释）
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
	// Enable Second Attribute of the vertex
	glEnableVertexAttribArray(1);
	// tell openGL how to interpret data
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (char*)(sizeof(float) * 3));


	// ELEMENT ARRAY
	// Define indices
	GLushort indices[] = { 0,1,1,2,2,3,3,0, 4,5,6, 5,6,7, 8,9,10, 9,10,11};
	// Create a index buffer ID
	GLuint indexBufferID;
	// Create a index buffer
	glGenBuffers(1, &indexBufferID);
	// Bind to ELEMENT ARRAY BUFFER
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
	// Pass data to ELEMENT ARRAY
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

bool checkStatus(GLuint objectID, PFNGLGETSHADERIVPROC objectPropertyGetter, PFNGLGETSHADERINFOLOGPROC getInfoLogFunc, GLenum statusType)
{
	GLint status;
	objectPropertyGetter(objectID, statusType, &status);
	if (status != GL_TRUE)
	{
		GLint infoLogLength;
		objectPropertyGetter(objectID, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar* buffer = new GLchar[infoLogLength];

		GLsizei bufferSize;
		getInfoLogFunc(objectID, infoLogLength, &bufferSize, buffer);
		std::cout << buffer << std::endl;
		delete[] buffer;
		return false;
	}
	return true;
}

bool checkShaderStatus(GLuint shaderID)
{
	return checkStatus(shaderID, glGetShaderiv, glGetShaderInfoLog, GL_COMPILE_STATUS);
}

bool checkProgramStatus(GLuint programID)
{	
	return checkStatus(programID, glGetProgramiv, glGetProgramInfoLog, GL_LINK_STATUS);
}

 std::string readShaderCode(const char* fileName)
{
	 std::ifstream myInput(fileName);
	 if (!myInput.good())
	 {
		 std::cout << "File failed to load... " << fileName << std::endl;
		 system("pause");
		 exit(0);
	 }
	 return std::string(
		 std::istreambuf_iterator<char>(myInput),
		 std::istreambuf_iterator<char>());
}

void installShaders()
{
	// Create ID for Shaders
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	
	// Create a char pointer to read the shader string
	const GLchar* adapter[1];
	std::string temp = readShaderCode("VertexShaderCode.glsl");
	adapter[0] = temp.c_str();
	// 0 means auto calulate when the compiler see a /0 null terminator
	glShaderSource(vertexShaderID, 1, adapter, 0);
	temp = readShaderCode("FragmentShaderCode.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(fragmentShaderID, 1, adapter, 0);

	glCompileShader(vertexShaderID);
	glCompileShader(fragmentShaderID);

	if (! checkShaderStatus(vertexShaderID) || ! checkShaderStatus(fragmentShaderID))
		return;

	programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);

	if (!checkProgramStatus(programID))
		return;

	glUseProgram(programID);
}

void MyGLWindow::paintGL()
{
	// Set Viewport Width, Height
	glViewport(0, 0, width(), height());
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	DrawStaic();
	DrawDynamic();
	DetectCollision();
}

void MyGLWindow::DrawDynamic()
{
	
	translate += (velocity / 5.0f) * ((float)(elapsedTimer.elapsed() - oldTime) / 1000.f);

	UpdateUniform(translate, color);
	// Draw ELEMENT ARRAY
	glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_SHORT,(void*)(8*sizeof(GLushort)));
	oldTime = elapsedTimer.elapsed();
}
void MyGLWindow::DrawStaic()
{
	// 开始绘制（绘制类型，第一个数据，多少个vertex渲染）
	//glDrawArrays(GL_LINES, 0, 4);
	UpdateUniform(staticPos, arenaColor);
	glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, 0);
}

void MyGLWindow::initializeGL()
{
	// Init glew
	glewInit();

	glEnable(GL_DEPTH_TEST);

	sendDataToOpenGL();

	installShaders();

	StartGame();
}

#pragma endregion RenderPart

#pragma region GameLoop

void MyGLWindow::keyPressEvent(QKeyEvent *e)
{
	switch (e->key())
	{
	case Qt::Key_W:
		StartGame();
		break;
	case Qt::Key_S:
		DetectCollision();
		break;
	case Qt::Key_Escape:
		close();
	}
}

void MyGLWindow::randomColor(float* Inputcolor)
{
	Inputcolor[0] = (double)rand() / (RAND_MAX);
	Inputcolor[1] = (double)rand() / (RAND_MAX);
	Inputcolor[2] = (double)rand() / (RAND_MAX);
}

void MyGLWindow::StartGame()
{
	translate = { 0.0,0.0 };

	staticPos = vec2(0.0);

 	velocity.x = ((float)rand() / (RAND_MAX) * 2 - 1);
	velocity.y = ((float)rand() / (RAND_MAX) * 2 - 1);
	glm::normalize(velocity);

	printf("%f Veclocity: %f,%f \n", (float)(elapsedTimer.elapsed() - oldTime) / 1000.f,velocity[0], velocity[1]);
	randomColor(color);
	arenaColor[0] = 1.0;
	arenaColor[1] = 1.0;
	arenaColor[2] = 1.0;
}

void MyGLWindow::UpdateUniform(vec2 TriTranslate, float randcolor[3])
{
	if (programID)
	{
		GLint translateID = glGetUniformLocation(programID, "translate");
		if (translateID != -1)
		{
			glUniform2fv(translateID, 1, &TriTranslate[0]);
		}
		GLint randomColID = glGetUniformLocation(programID, "randomCol");
		if (randomColID != -1)
		{
			float color[3] = { (double)rand() / (RAND_MAX),(double)rand() / (RAND_MAX),(double)rand() / (RAND_MAX)};
			glUniform3fv(randomColID, 1, randcolor);
		}
	}
	else
		std::cout << "Error: Can't find programID" << std::endl;
}

void MyGLWindow::DetectCollision()
{
	bool anyCollision = false;
	for (uint i = 0; i < 4; i++)
	{
		const vec2 first = vec2(Arena[i*6],Arena[i*6+1]);
		const vec2 second = vec2(Arena[(i+1)%4*6], Arena[(i + 1)%4 * 6 + 1]);
		vec2 wall = second - first;
		vec2 normal = glm::normalize(vec2(-wall.y,wall.x));

		float distance = glm::dot(normal,translate-first);
		printf("distance%d: %f \n",i, distance);
		//printf("normal %d: %f,%f \n",i, normal.x,normal.y);
		if (distance < 0)
		{
			velocity = velocity - 2 * glm::dot(normal,velocity) * normal;
		}
	}
}

#pragma endregion GameLogic

