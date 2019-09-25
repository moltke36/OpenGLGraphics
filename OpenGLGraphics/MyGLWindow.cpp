#include <GL\glew.h>
#include <iostream>
#include <Qt3DInput/qkeyevent.h>
#include <QtCore/qelapsedtimer.h>
#include <stdlib.h> 
#include <fstream>
#include <QtCore\qtimer.h>
#include "MyGLWindow.h"

int numTris = 0;
float translate[2];
float staticPos[2];
float color[3];
float arenaColor[3];
bool endGame = false;
float velocity[2];
int oldTime;
QElapsedTimer elapsedTimer;
GLuint programID;

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

	// Create verts
	GLfloat Arena[] =
	{
		+0.0f, +1.0f, RED_TRIANGLE_Z,
		+1.0f, +0.0f, +0.0f,
		-1.0f, +0.0f, RED_TRIANGLE_Z,
		+1.0f, +0.0f, +0.0f,
		+0.0f, -1.0f, RED_TRIANGLE_Z,
		+1.0f, +0.0f, +0.0f,
		+1.0f, +0.0f, RED_TRIANGLE_Z,
		+1.0f, +0.0f, +0.0f,
	};

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
}

void MyGLWindow::DrawDynamic()
{
	
	translate[0] += (velocity[0] / 5.0f) * ((float)(elapsedTimer.elapsed() - oldTime) / 1000.f);
	translate[1] += (velocity[1] / 5.0f) * ((float)(elapsedTimer.elapsed() - oldTime) / 1000.f);

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
		translate[1] += -0.1f;
		randomColor(color);
		break;
	case Qt::Key_A:
		translate[0] += -0.1f;
		randomColor(color);
		break;
	case Qt::Key_D:
		translate[0] += +0.1f;
		randomColor(color);
		break;
	case Qt::Key_Up:
		translate[1] += +0.1f;
		randomColor(color);
		break;
	case Qt::Key_Down:
		translate[1] += -0.1f;
		randomColor(color);
		break;
	case Qt::Key_Left:
		translate[0] += -0.1f;
		randomColor(color);
		break;
	case Qt::Key_Right:
		translate[0] += +0.1f;
		randomColor(color);
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
	translate[0] = 0.0;
	translate[1] = 0.0;

	staticPos[0] = 0.0;
	staticPos[1] = 0.0;

 	velocity[0] = ((double)rand() / (RAND_MAX) * 2 - 1);
	velocity[1] = ((double)rand() / (RAND_MAX) * 2 - 1);

	printf("%f Veclocity: %f,%f \n", (float)(elapsedTimer.elapsed() - oldTime) / 1000.f,velocity[0], velocity[1]);
	randomColor(color);
	arenaColor[0] = 1.0;
	arenaColor[1] = 1.0;
	arenaColor[2] = 1.0;
}

void MyGLWindow::UpdateUniform(float TriTranslate[2], float randcolor[3])
{
	if (programID)
	{
		GLint translateID = glGetUniformLocation(programID, "translate");
		if (translateID != -1)
		{
			glUniform2fv(translateID, 1, TriTranslate);
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

#pragma endregion GameLogic

