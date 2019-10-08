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
using glm::vec4;

const uint NUM_VERTICES_PER_TRI = 3;
const uint NUM_FLOATS_PER_VERTICE = 9;
const uint VERTEX_BYTE_SIZE = NUM_FLOATS_PER_VERTICE * sizeof(float);

bool endGame = false;
GLuint programID;

GLuint theBufferID;

GLuint teapotNumIndices;
GLuint teapotNormalNumIndices;

GLuint arrowNumIndices;
GLuint arrowNormalNumIndices;

GLuint planeNumIndices;
GLuint planeNormalNumIndices;

GLuint teapotVertexArrayObjectID;
GLuint arrowVertexArrayObjectID;
GLuint planeVertexArrayObjectID;

GLuint arrowIndexDataByteOffset;
GLuint teapotIndexDataByteOffset;
GLuint planeIndexDataByteOffset;

GLuint teapotNormalsVertexArrayObjectID;
GLuint arrowNormalsVertexArrayObjectID;
GLuint planeNormalsVertexArrayObjectID;
GLuint teapotNormalsIndexDataByteOffset;
GLuint arrowNormalsIndexDataByteOffset;
GLuint planeNormalsIndexDataByteOffset;

Camera camera;
GLuint modelToProjectionMatrixUniformLocation;


MyGLWindow::MyGLWindow()
{
	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(update()));
	timer->start(15);

	setWindowTitle(tr("Move Triangles"));
}

MyGLWindow::~MyGLWindow()
{
	glDeleteBuffers(1, &theBufferID);

	glUseProgram(0);
	glDeleteProgram(programID);
}

#pragma region OpenGL

void MyGLWindow::sendDataToOpenGL()
{
	ShapeData teapot = ShapeGenerator::makeTeapot(10);
	ShapeData teapotNormals = ShapeGenerator::generateNormals(teapot);
	ShapeData arrow = ShapeGenerator::makeArrow();
	ShapeData arrowNormals = ShapeGenerator::generateNormals(arrow);
	ShapeData plane = ShapeGenerator::makePlane(20);
	ShapeData planeNormals = ShapeGenerator::generateNormals(plane);
	// Create a BufferID

	// Create a Buffer Object with myID
	glGenBuffers(1, &theBufferID);
	// 必须要选一个Buffer类型绑定，ELE MENT_ARRAY OR ARRAY
	//（或许可以理解成Buffer object连接上ARRAY）
	glBindBuffer(GL_ARRAY_BUFFER, theBufferID);
	// Send Data to Buffer(哪个buff，data大小，data, 送到哪里）
	glBufferData(GL_ARRAY_BUFFER, 
		teapot.vertexBufferSize() + teapot.indexBufferSize() +
		plane.vertexBufferSize() + plane.indexBufferSize() +
		arrow.vertexBufferSize() + arrow.indexBufferSize() +
		teapotNormals.vertexBufferSize() + teapotNormals.indexBufferSize() +
		arrowNormals.vertexBufferSize() + arrowNormals.indexBufferSize() +
		planeNormals.vertexBufferSize() + planeNormals.indexBufferSize(), 0, GL_STATIC_DRAW);
	// Enable Vertex Attribute (vertex有很多不同属性，位置是一个);

	GLsizeiptr currentOffset = 0;

	// Mesh
	glBufferSubData(GL_ARRAY_BUFFER, currentOffset, teapot.vertexBufferSize(), teapot.vertices);
	currentOffset += teapot.vertexBufferSize();
	teapotIndexDataByteOffset = currentOffset;
	glBufferSubData(GL_ARRAY_BUFFER, currentOffset, teapot.indexBufferSize(), teapot.indices);
	currentOffset += teapot.indexBufferSize();
	glBufferSubData(GL_ARRAY_BUFFER, currentOffset, arrow.vertexBufferSize(), arrow.vertices);
	currentOffset += arrow.vertexBufferSize();
	arrowIndexDataByteOffset = currentOffset;
	glBufferSubData(GL_ARRAY_BUFFER, currentOffset, arrow.indexBufferSize(), arrow.indices);
	currentOffset += arrow.indexBufferSize();
	glBufferSubData(GL_ARRAY_BUFFER, currentOffset, plane.vertexBufferSize(), plane.vertices);
	currentOffset += plane.vertexBufferSize();
	planeIndexDataByteOffset = currentOffset;
	glBufferSubData(GL_ARRAY_BUFFER, currentOffset, plane.indexBufferSize(), plane.indices);
	currentOffset += plane.indexBufferSize();
	// Normal
	glBufferSubData(GL_ARRAY_BUFFER, currentOffset, teapotNormals.vertexBufferSize(), teapotNormals.vertices);
	currentOffset += teapotNormals.vertexBufferSize();
	teapotNormalsIndexDataByteOffset = currentOffset;
	glBufferSubData(GL_ARRAY_BUFFER, currentOffset, teapotNormals.indexBufferSize(), teapotNormals.indices);
	currentOffset += teapotNormals.indexBufferSize();
	glBufferSubData(GL_ARRAY_BUFFER, currentOffset, arrowNormals.vertexBufferSize(), arrowNormals.vertices);
	currentOffset += arrowNormals.vertexBufferSize();
	arrowNormalsIndexDataByteOffset = currentOffset;
	glBufferSubData(GL_ARRAY_BUFFER, currentOffset, arrowNormals.indexBufferSize(), arrowNormals.indices);
	currentOffset += arrowNormals.indexBufferSize();
	glBufferSubData(GL_ARRAY_BUFFER, currentOffset, planeNormals.vertexBufferSize(), planeNormals.vertices);
	currentOffset += planeNormals.vertexBufferSize();
	planeNormalsIndexDataByteOffset = currentOffset;
	glBufferSubData(GL_ARRAY_BUFFER, currentOffset, planeNormals.indexBufferSize(), planeNormals.indices);
	currentOffset += planeNormals.indexBufferSize();

	teapotNumIndices = teapot.numIndices;
	teapotNormalNumIndices = teapotNormals.numIndices;
	arrowNumIndices = arrow.numIndices;
	arrowNormalNumIndices = arrowNormals.numIndices;
	planeNumIndices = plane.numIndices;
	planeNormalNumIndices = planeNormals.numIndices;

	glGenVertexArrays(1, &teapotVertexArrayObjectID);
	glGenVertexArrays(1, &arrowVertexArrayObjectID);
	glGenVertexArrays(1, &planeVertexArrayObjectID);
	glGenVertexArrays(1, &teapotNormalsVertexArrayObjectID);
	glGenVertexArrays(1, &arrowNormalsVertexArrayObjectID);
	glGenVertexArrays(1, &planeNormalsVertexArrayObjectID);

	// teapot
	glBindVertexArray(teapotVertexArrayObjectID);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, theBufferID);
	// 解释Vertex数据（位置，两个数据一个点，GLFloat类型，禁用Normalize，之后解释，之后解释）
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, 0);
	// tell openGL how to interpret data
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(sizeof(float) * 3));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(sizeof(float) * 6));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, theBufferID);

	// Arrow Right here
	glBindVertexArray(arrowVertexArrayObjectID);
	// Enable Vertex Attribute (vertex有很多不同属性，位置是一个);
	glEnableVertexAttribArray(0);
	// Enable Second Attribute of the vertex
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, theBufferID);
	GLuint arrowByteOffset = teapot.vertexBufferSize() + teapot.indexBufferSize();
	// 解释Vertex数据（位置，两个数据一个点，GLFloat类型，禁用Normalize，之后解释，之后解释）
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)arrowByteOffset);
	// tell openGL how to interpret data
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(arrowByteOffset + sizeof(float) * 3));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(arrowByteOffset + sizeof(float) * 6));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, theBufferID);

	// Plane
	glBindVertexArray(planeVertexArrayObjectID);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, theBufferID);
	GLuint planeByteOffset = arrowByteOffset + arrow.vertexBufferSize() + arrow.indexBufferSize();
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)planeByteOffset);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(planeByteOffset + sizeof(float) * 3));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(planeByteOffset + sizeof(float) * 6));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, theBufferID);

	// Teapot Normal
	glBindVertexArray(teapotNormalsVertexArrayObjectID);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, theBufferID);
	GLuint teapotNormalByteOffset = planeByteOffset + plane.vertexBufferSize() + plane.indexBufferSize();
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)teapotNormalByteOffset);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(teapotNormalByteOffset + sizeof(float) *3));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, theBufferID);

	// arrow Normal
	glBindVertexArray(arrowNormalsVertexArrayObjectID);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, theBufferID);
	GLuint arrowNormalByteOffset = teapotNormalByteOffset + teapotNormals.vertexBufferSize() + teapotNormals.indexBufferSize();
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)arrowNormalByteOffset);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(arrowNormalByteOffset + sizeof(float) * 3));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, theBufferID);

	// plane Normal
	glBindVertexArray(planeNormalsVertexArrayObjectID);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, theBufferID);
	GLuint planeNormalByteOffset = arrowNormalByteOffset + arrowNormals.vertexBufferSize() + arrowNormals.indexBufferSize();
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)planeNormalByteOffset);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(planeNormalByteOffset + sizeof(float) * 3));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, theBufferID);

	teapot.cleanup();
	arrow.cleanup();
	plane.cleanup();
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
	//GLint transformLocation = glGetAttribLocation(programID, "modelToProjectionMatrix");

	// Since it's attached, we no longer need shader code in our memory.
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	glUseProgram(programID);
}

void MyGLWindow::paintGL()
{
	glViewport(0, 0, width(), height());
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	mat4 modelToProjectionMatrix;
	// Projection Matrix （笔记PMatrix 物体压扁）
	mat4 viewToProjectionMatrix = glm::perspective(60.0f* (M_PI / 180.0f), ((GLfloat)width() / (GLfloat)height()), 0.1f, 150.0f);
	mat4 worldToViewMatrix = camera.getWorldToViewMatrix();
	mat4 worldToProjectionMatrix = viewToProjectionMatrix * worldToViewMatrix;

	GLuint lightPositionWorldUniformLocation = glGetUniformLocation(programID, "lightPositionWorld");
	vec3 lightPositionWorld(0.0f, 5.0f, 0.0f);
	glUniform3fv(lightPositionWorldUniformLocation, 1, &lightPositionWorld[0]);
	GLuint eyePositionWorldUniformLocation = glGetUniformLocation(programID, "eyePositionWorld");
	glm::vec3 eyePosition = camera.getPosition();
	glUniform3fv(eyePositionWorldUniformLocation, 1, &eyePosition[0]);
	GLuint ambientLightUniformLocation = glGetUniformLocation(programID, "ambientLight");
	vec4 ambientLight(0.15f, 0.15f, 0.15f,1.0f);
	glUniform4fv(ambientLightUniformLocation, 1, &ambientLight[0]);

	GLuint modelToWorldMatrixUniformLocation = glGetUniformLocation(programID, "modelToWorldMatrix");

	glBindVertexArray(teapotVertexArrayObjectID);
	mat4 teapot1ModelToWorldMatrix =
		glm::translate(vec3(-3.0f, 0.0f, -8.0f)) *
		glm::rotate(-90.0f* (M_PI / 180.0f), vec3(1.0f, 0.0f, 0.0f));
	modelToProjectionMatrix = worldToProjectionMatrix * teapot1ModelToWorldMatrix;
	glUniformMatrix4fv(modelToProjectionMatrixUniformLocation, 1, GL_FALSE, &modelToProjectionMatrix[0][0]);
	glUniformMatrix4fv(modelToWorldMatrixUniformLocation, 1, GL_FALSE, &teapot1ModelToWorldMatrix[0][0]);
	glDrawElements(GL_TRIANGLES, teapotNumIndices, GL_UNSIGNED_SHORT, (void*)teapotIndexDataByteOffset);
	//glBindVertexArray(teapotNormalsVertexArrayObjectID);
	//glDrawElements(GL_LINES, teapotNormalNumIndices, GL_UNSIGNED_SHORT, (void*)teapotNormalsIndexDataByteOffset);
	
	glBindVertexArray(teapotVertexArrayObjectID);
	mat4 teapot2ModelToWorldMatrix =
		glm::translate(vec3(3.0f, 0.0f, 3.75f)) *
		glm::rotate(-90.0f* (M_PI / 180.0f), vec3(1.0f, 0.0f, 0.0f));
	modelToProjectionMatrix = worldToProjectionMatrix * teapot2ModelToWorldMatrix;
	glUniformMatrix4fv(modelToProjectionMatrixUniformLocation, 1, GL_FALSE, &modelToProjectionMatrix[0][0]);
	glUniformMatrix4fv(modelToWorldMatrixUniformLocation, 1, GL_FALSE, &teapot2ModelToWorldMatrix[0][0]);
	glDrawElements(GL_TRIANGLES, teapotNumIndices, GL_UNSIGNED_SHORT, (void*)teapotIndexDataByteOffset);
	//glBindVertexArray(teapotNormalsVertexArrayObjectID);
	//glDrawElements(GL_LINES, teapotNormalNumIndices, GL_UNSIGNED_SHORT, (void*)teapotNormalsIndexDataByteOffset);

	

	glBindVertexArray(arrowVertexArrayObjectID);
	mat4 arrowModelToWorldMatrix =
		glm::translate(vec3(0.0f, 2.0f, -8.00f)) *
		glm::rotate(90.0f, vec3(1.0f, 0.0f, 0.0f));
	modelToProjectionMatrix = worldToProjectionMatrix * arrowModelToWorldMatrix;
	glUniformMatrix4fv(modelToProjectionMatrixUniformLocation, 1, GL_FALSE, &modelToProjectionMatrix[0][0]);
	glUniformMatrix4fv(modelToWorldMatrixUniformLocation, 1, GL_FALSE, &arrowModelToWorldMatrix[0][0]);
	glDrawElements(GL_TRIANGLES, arrowNumIndices, GL_UNSIGNED_SHORT, (void*)arrowIndexDataByteOffset);
	//glBindVertexArray(arrowNormalsVertexArrayObjectID);
	//glDrawElements(GL_LINES, arrowNormalNumIndices, GL_UNSIGNED_SHORT, (void*)arrowNormalsIndexDataByteOffset);
	
	//arrow center
	arrowModelToWorldMatrix =
		glm::translate(vec3(0.0f, 0.0f, 0.00f)) *
		glm::rotate(0.0f, vec3(1.0f, 0.0f, 0.0f));
	modelToProjectionMatrix = worldToProjectionMatrix * arrowModelToWorldMatrix;
	glUniformMatrix4fv(modelToProjectionMatrixUniformLocation, 1, GL_FALSE, &modelToProjectionMatrix[0][0]);
	glUniformMatrix4fv(modelToWorldMatrixUniformLocation, 1, GL_FALSE, &arrowModelToWorldMatrix[0][0]);
	glDrawElements(GL_TRIANGLES, arrowNumIndices, GL_UNSIGNED_SHORT, (void*)arrowIndexDataByteOffset);

	glBindVertexArray(planeVertexArrayObjectID);
	mat4 planeModelToWorldMatrix =
		glm::translate(vec3(0.0f, 0.0f, -3.00f)) *
		glm::rotate(0.0f, vec3(1.0f, 0.0f, 0.0f));
	modelToProjectionMatrix = worldToProjectionMatrix * planeModelToWorldMatrix;
	glUniformMatrix4fv(modelToProjectionMatrixUniformLocation, 1, GL_FALSE, &modelToProjectionMatrix[0][0]);
	glDrawElements(GL_TRIANGLES, planeNumIndices, GL_UNSIGNED_SHORT, (void*)planeIndexDataByteOffset);
	//glBindVertexArray(planeNormalsVertexArrayObjectID);
	//glDrawElements(GL_LINES, planeNormalNumIndices, GL_UNSIGNED_SHORT, (void*)planeNormalsIndexDataByteOffset);
	
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

	glEnable(GL_CULL_FACE);

	glCullFace(GL_BACK);

	sendDataToOpenGL();

	installShaders();

	StartGame();

	modelToProjectionMatrixUniformLocation = glGetUniformLocation(programID, "modelToProjectionMatrix");
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

