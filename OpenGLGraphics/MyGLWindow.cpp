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
GLuint theVertexBufferID;
GLuint theIndexBufferID;

GLuint cubeNumIndices;
GLuint arrowNumIndices;
GLuint cubeVertexArrayObjectID;
GLuint arrowVertexArrayObjectID;
GLuint arrowIndexDataByteOffset;

Camera camera;
GLuint fullTransformationUniformLocation;

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
	ShapeData cube = ShapeGenerator::makeCube();
	ShapeData arrow = ShapeGenerator::makeArrow();
	// Create a BufferID

	// Create a Buffer Object with myID
	glGenBuffers(1, &theVertexBufferID);
	// 必须要选一个Buffer类型绑定，ELEMENT_ARRAY OR ARRAY
	//（或许可以理解成Buffer object连接上ARRAY）
	glBindBuffer(GL_ARRAY_BUFFER, theVertexBufferID);
	// Send Data to Buffer(哪个buff，data大小，data, 送到哪里）
	glBufferData(GL_ARRAY_BUFFER, cube.vertexBufferSize() + arrow.vertexBufferSize(), 0, GL_STATIC_DRAW);
	// Enable Vertex Attribute (vertex有很多不同属性，位置是一个);
	glBufferSubData(GL_ARRAY_BUFFER, 0, cube.vertexBufferSize(), cube.vertices);
	glBufferSubData(GL_ARRAY_BUFFER, cube.vertexBufferSize(), arrow.vertexBufferSize(), arrow.vertices);
	glEnableVertexAttribArray(0);
	// Enable Second Attribute of the vertex
	glEnableVertexAttribArray(1);
	//// 解释Vertex数据（位置，两个数据一个点，GLFloat类型，禁用Normalize，之后解释，之后解释）
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
	//// tell openGL how to interpret data
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (char*)(sizeof(float) * 3));


	// ELEMENT ARRAY
	// Define indices
	//GLushort indices[] = { 0,1,2,3,4,5 };
	// Create a index buffer
	glGenBuffers(1, &theIndexBufferID);
	// Bind to ELEMENT ARRAY BUFFER
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, theIndexBufferID);
	// Pass data to ELEMENT ARRAY
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, cube.indexBufferSize() + arrow.indexBufferSize(), 0, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, cube.indexBufferSize(), cube.indices);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, cube.indexBufferSize(), arrow.indexBufferSize(), arrow.indices);
	cubeNumIndices = cube.numIndices;
	arrowNumIndices = arrow.numIndices;


	glGenVertexArrays(1, &cubeVertexArrayObjectID);
	glGenVertexArrays(1, &arrowVertexArrayObjectID);

	// Cube
	glBindVertexArray(cubeVertexArrayObjectID);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, theVertexBufferID);
	// 解释Vertex数据（位置，两个数据一个点，GLFloat类型，禁用Normalize，之后解释，之后解释）
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
	// tell openGL how to interpret data
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (char*)(sizeof(float) * 3));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, theIndexBufferID);

	// Arrow Right here
	glBindVertexArray(arrowVertexArrayObjectID);
	// Enable Vertex Attribute (vertex有很多不同属性，位置是一个);
	glEnableVertexAttribArray(0);
	// Enable Second Attribute of the vertex
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, theVertexBufferID);
	// 解释Vertex数据（位置，两个数据一个点，GLFloat类型，禁用Normalize，之后解释，之后解释）
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)cube.vertexBufferSize());
	// tell openGL how to interpret data
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(cube.vertexBufferSize()+sizeof(float) * 3));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, theIndexBufferID);

	arrowIndexDataByteOffset = cube.indexBufferSize();

	cube.cleanup();
	arrow.cleanup();
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
	glViewport(0, 0, width(), height());
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	mat4 fullTransformMatrix;
	// Projection Matrix （笔记PMatrix 物体压扁）
	mat4 viewToProjectionMatrix = glm::perspective(60.0f* (M_PI / 180.0f), ((GLfloat)width() / (GLfloat)height()), 0.1f, 150.0f);
	mat4 worldToViewMatrix = camera.getWorldToViewMatrix();
	mat4 worldToProjectionMatrix = viewToProjectionMatrix * worldToViewMatrix;



	glBindVertexArray(cubeVertexArrayObjectID);
	mat4 cube1ModelToWorldMatrix =
		glm::translate(vec3(-2.0f, 0.0f, -3.0f)) *
		glm::rotate(36.0f, vec3(1.0f, 0.0f, 0.0f));
	fullTransformMatrix = worldToProjectionMatrix * cube1ModelToWorldMatrix;
	glUniformMatrix4fv(fullTransformationUniformLocation, 1, GL_FALSE, &fullTransformMatrix[0][0]);
	glDrawElements(GL_TRIANGLES, cubeNumIndices, GL_UNSIGNED_SHORT, 0);

	mat4 cube2ModelToWorldMatrix =
		glm::translate(vec3(2.0f, 0.0f, -3.75f)) *
		glm::rotate(126.0f, vec3(0.0f, 1.0f, 0.0f));
	fullTransformMatrix = worldToProjectionMatrix * cube2ModelToWorldMatrix;
	glUniformMatrix4fv(fullTransformationUniformLocation, 1, GL_FALSE, &fullTransformMatrix[0][0]);
	glDrawElements(GL_TRIANGLES, cubeNumIndices, GL_UNSIGNED_SHORT, 0);

	glBindVertexArray(arrowVertexArrayObjectID);
	mat4 arrowModelToWorldMatrix =
		glm::translate(vec3(0.0f, 0.0f, -3.00f)) *
		glm::rotate(126.0f, vec3(0.0f, 1.0f, 0.0f));
	fullTransformMatrix = worldToProjectionMatrix * arrowModelToWorldMatrix;
	glUniformMatrix4fv(fullTransformationUniformLocation, 1, GL_FALSE, &fullTransformMatrix[0][0]);
	glDrawElements(GL_TRIANGLES, arrowNumIndices, GL_UNSIGNED_SHORT, (void*)arrowIndexDataByteOffset);

	Update();
	Draw();
}

void MyGLWindow::Draw()
{
	// Set Viewport Width, Height


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

	fullTransformationUniformLocation = glGetUniformLocation(programID, "fullTransformMatrix");
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

