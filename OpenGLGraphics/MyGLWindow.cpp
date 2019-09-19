﻿#include <GL\glew.h>
#include <iostream>
#include <Qt3DInput/qkeyevent.h>
#include <stdlib.h> 
#include <fstream>
#include <QtCore\qtimer.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtx/transform.hpp>
#include <Vertex.h>
#include <ShapeGenerator.h>
#include <stdio.h>
#include "MyGLWindow.h"
using glm::vec3;
using glm::mat4;


const float Y_DELTA = 0.2f;
const uint NUM_VERTICES_PER_TRI = 3;
const uint NUM_FLOATS_PER_VERTICE = 6;
const uint TRIANGLE_BYTE_SIZE = NUM_VERTICES_PER_TRI * NUM_FLOATS_PER_VERTICE * sizeof(float);
const uint MAX_TRIS = 40;

int numTris = 0;
float translateLeft[2];
float translateRight[2];
float leftcolor[3];
float rightcolor[3];
bool endGame = false;
GLuint programID;
GLuint numIndices;

MyGLWindow::MyGLWindow()
{
	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(update()));
	timer->start(32);

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
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, shape.indexBufferSize(), shape.indices, GL_STATIC_DRAW);
	numIndices = shape.numIndices;
	shape.cleanup();
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

	// Since it's attached, we no longer need shader code in our memory.
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);


	if (!checkProgramStatus(programID))
		return;

	glUseProgram(programID);
}

void MyGLWindow::paintGL()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	Update(translateLeft,leftcolor);
	Draw();
	//Update(translateRight,rightcolor);
	//Draw();
}

void MyGLWindow::Draw()
{
	// Set Viewport Width, Height
	glViewport(0, 0, width(), height());

	mat4 fullTransformMatrix;

	// Projection Matrix （笔记PMatrix 物体压扁）
	mat4 projectionMatrix = glm::perspective(30.0f, ((float)width()) / height(), 0.1f, 10.0f);

	// Cube 1:
	// Model到World Matrix
	mat4 translationMatrix = glm::translate(vec3(-1.0f, 0.0f, -3.0f));
	// Model到World Matrix 
	mat4 rotationMatrix = glm::rotate(36.0f + translateLeft[0], vec3(1.0f, 0.0f, 0.0f));

	fullTransformMatrix = projectionMatrix * translationMatrix * rotationMatrix;

	GLint fullTransformMatrixUniformLocation =
		glGetUniformLocation(programID, "fullTransformMatrix"); 

	glUniformMatrix4fv(fullTransformMatrixUniformLocation, 1, GL_FALSE, &fullTransformMatrix[0][0]);

	// 开始绘制（绘制类型，第一个数据，多少个vertex渲染）
	//glDrawArrays(GL_TRIANGLES, 0, 3);
	// Draw ELEMENT ARRAY
	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, 0);

	// Cube 2:
	// Model到World Matrix
	translationMatrix = glm::translate(vec3(1.0f, 0.0f, -3.75f));
	// Model到World Matrix 
	rotationMatrix = glm::rotate(126.0f + translateLeft[0], vec3(0.0f, 1.0f, 0.0f));

	fullTransformMatrix = projectionMatrix * translationMatrix * rotationMatrix;

	glUniformMatrix4fv(fullTransformMatrixUniformLocation, 1, GL_FALSE, &fullTransformMatrix[0][0]);

	// Draw ELEMENT ARRAY
	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, 0);
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
		translateLeft[1] += +0.1f;
		randomColor(leftcolor);
		break;
	case Qt::Key_S:
		translateLeft[1] += -0.1f;
		randomColor(leftcolor);
		break;
	case Qt::Key_A:
		translateLeft[0] += -0.1f;
		randomColor(leftcolor);
		break;
	case Qt::Key_D:
		translateLeft[0] += +0.1f;
		randomColor(leftcolor);
		break;
	case Qt::Key_Up:
		translateRight[1] += +0.1f;
		randomColor(rightcolor);
		break;
	case Qt::Key_Down:
		translateRight[1] += -0.1f;
		randomColor(rightcolor);
		break;
	case Qt::Key_Left:
		translateRight[0] += -0.1f;
		randomColor(rightcolor);
		break;
	case Qt::Key_Right:
		translateRight[0] += +0.1f;
		randomColor(rightcolor);
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
	translateLeft[0] = 0.0;
	translateLeft[1] = 0.0;
	translateRight[0] = 0.0;
	translateRight[1] = 0.0;

	leftcolor[0] = 1.0;
	leftcolor[1] = 1.0;
	leftcolor[2] = 1.0;

	rightcolor[0] = 1.0;
	rightcolor[1] = 1.0;
	rightcolor[2] = 1.0;
}

void MyGLWindow::Update(float TriTranslate[2], float randcolor[3])
{
	if (programID)
	{
		GLint translate = glGetUniformLocation(programID, "translate");
		if (translate != -1)
		{
			glUniform2fv(translate, 1, TriTranslate);
		}
		GLint randomCol = glGetUniformLocation(programID, "randomCol");
		if (randomCol != -1)
		{
			float color[3] = { (float)rand() / (RAND_MAX),(float)rand() / (RAND_MAX),(float)rand() / (RAND_MAX)};
			glUniform3fv(randomCol, 1, randcolor);
		}
	}
	else
		std::cout << "Error: Can't find programID" << std::endl;
}

#pragma endregion GameLogic

