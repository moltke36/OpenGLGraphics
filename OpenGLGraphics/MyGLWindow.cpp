#include <GL\glew.h>
#include <iostream>
#include <Qt3DInput/qkeyevent.h>
//#include <Qt3DInput/qmouseevent.h>
#include <stdlib.h> 
#include <fstream>
#include <QtCore\qtimer.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtx/transform.hpp>
#include <Vertex.h>
#include <ShapeGenerator.h>
#include <stdio.h>
#include <Camera.h>
#include "MyGLWindow.h"

#define M_PI 3.14159265359f

using glm::vec3;
using glm::mat4;

const float Y_DELTA = 0.2f;

bool endGame = false;
GLuint programID;
GLuint numIndices;

Camera camera;

MyGLWindow::MyGLWindow()
{
	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(update()));
	timer->start(15);

	setWindowTitle(tr("Move Triangles"));
}

MyGLWindow::~MyGLWindow()
{
	glUseProgram(0);
	glDeleteProgram(programID);
}

#pragma region OpenGL



void MyGLWindow::sendDataToOpenGL()
{
	ShapeData shape = ShapeGenerator::makeCube();

	// Create a BufferID
	GLuint vertexBufferID;
	// Create a Buffer Object with myID
	glGenBuffers(1, &vertexBufferID);
	// 必须要选一个Buffer类型绑定，ELEMENT_ARRAY OR ARRAY
	//（或许可以理解成Buffer object连接上ARRAY）
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	// Send Data to Buffer(哪个buff，data大小，data, 送到哪里）
	glBufferData(GL_ARRAY_BUFFER, shape.vertexBufferSize(), shape.vertices, GL_STATIC_DRAW);
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
	//GLushort indices[] = { 0,1,2,3,4,5 };
	// Create a index buffer ID
	GLuint indexBufferID;
	// Create a index buffer
	glGenBuffers(1, &indexBufferID);
	// Bind to ELEMENT ARRAY BUFFER
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
	// Pass data to ELEMENT ARRAY
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, shape.indexBufferSize(), shape.indices, GL_DYNAMIC_DRAW);
	numIndices = shape.numIndices;
	shape.cleanup();

	GLuint tranformationMatrixBufferID;
	glGenBuffers(1, &tranformationMatrixBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, tranformationMatrixBufferID);



	glBufferData(GL_ARRAY_BUFFER, sizeof(mat4)*2, 0, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(float) * 0));
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(float) * 4));
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(float) * 8));
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(float) * 12));
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);
	glEnableVertexAttribArray(5);
	glVertexAttribDivisor(2, 1);
	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
}

bool MyGLWindow::checkStatus(GLuint objectID, PFNGLGETSHADERIVPROC objectPropertyGetter, PFNGLGETSHADERINFOLOGPROC getInfoLogFunc, GLenum statusType)
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

bool MyGLWindow::checkShaderStatus(GLuint shaderID)
{
	return checkStatus(shaderID, glGetShaderiv, glGetShaderInfoLog, GL_COMPILE_STATUS);
}

bool MyGLWindow::checkProgramStatus(GLuint programID)
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

void MyGLWindow::installShaders()
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
	// 可手动bind Attribute Location
	// glBindAttribLocation(programID,2,"position");
	glLinkProgram(programID);

	if (!checkProgramStatus(programID))
		return;

	//GLint positionLocation = glGetAttribLocation(programID, "position");
	//GLint colorLocation = glGetAttribLocation(programID, "vertexColor");
	//GLint transformLocation = glGetAttribLocation(programID, "fullTransformMatrix");

	// Since it's attached, we no longer need shader code in our memory.
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	glUseProgram(programID);
}

void MyGLWindow::paintGL()
{
	// Projection Matrix （笔记PMatrix 物体压扁）
	mat4 projectionMatrix = glm::perspective(60.0f* (M_PI/180.0f), ((GLfloat)width() / (GLfloat)height()), 0.1f, 150.0f);
	mat4 fullTransforms[] =
	{
		projectionMatrix * camera.getWorldToViewMatrix() * glm::translate(vec3(-1.0f, 0.0f, -3.0f)) * glm::rotate(36.0f, vec3(1.0f, 0.0f, 0.0f)),
		projectionMatrix * camera.getWorldToViewMatrix() * glm::translate(vec3(1.0f, 0.0f, -3.75f)) * glm::rotate(126.0f, vec3(0.0f, 1.0f, 0.0f))
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(fullTransforms), fullTransforms, GL_DYNAMIC_DRAW);

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	Update();
	Draw();
}

void MyGLWindow::Draw()
{
	// Set Viewport Width, Height
	glViewport(0, 0, width(), height());


	// Draw ELEMENT ARRAY
	glDrawElementsInstanced(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, 0,2);
}

void MyGLWindow::mouseMoveEvent(QMouseEvent* e)
{
	if (e->buttons() == Qt::RightButton)
	{
		camera.mouseUpdate(vec2(e->x(), e->y()));
	}
}

void MyGLWindow::initializeGL()
{
	// Enable Mouse Tracking
	setMouseTracking(true);

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
		camera.moveForward(1.0);
		break;
	case Qt::Key_S:
		camera.moveForward(-1.0);
		break;
	case Qt::Key_A:
		camera.strafeLeft(1.0);
		break;
	case Qt::Key_D:
		camera.strafeLeft(-1.0);
		break;
	case Qt::Key_Escape:
		close();
	}
}

void MyGLWindow::StartGame()
{

}

void MyGLWindow::Update()
{

}

#pragma endregion GameLogic

