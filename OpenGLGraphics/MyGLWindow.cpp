#include <GL\glew.h>
#include <iostream>
#include <Qt3DInput/qkeyevent.h>
#include <fstream>
#include "MyGLWindow.h"

const float Y_DELTA = 0.2f;
const uint NUM_VERTICES_PER_TRI = 3;
const uint NUM_FLOATS_PER_VERTICE = 6;
const uint TRIANGLE_BYTE_SIZE = NUM_VERTICES_PER_TRI * NUM_FLOATS_PER_VERTICE * sizeof(float);
const uint MAX_TRIS = 40;

int numTris = 0;



void MyGLWindow::sendDataToOpenGL()
{
	const float RED_TRIANGLE_Z = 0.5f;
	const float BLUE_TRIANBLE_Z = -0.5f;

	// Create verts
	//GLfloat verts[] =
	//{
	//	-1.0f,-1.0f, RED_TRIANGLE_Z,
	//	+1.0f, +0.0f, +0.0f,
	//	+0.0f,+1.0f, -1.0f,
	//	+1.0f, +1.0f, +1.0f,
	//	+1.0f,-1.0f, RED_TRIANGLE_Z,
	//	+1.0f, +0.0f, +0.0f,

	//	-1.0f,+1.0f,BLUE_TRIANBLE_Z,
	//	+0.0f, +0.0f, +1.0f,
	//	+0.0f,-1.0f,BLUE_TRIANBLE_Z,
	//	+0.0f, +0.0f, +1.0f,
	//	+1.0f,+1.0f,BLUE_TRIANBLE_Z,
	//	+0.0f, +0.0f, +1.0f
	//};

	// Create a BufferID
	GLuint vertexBufferID;
	// Create a Buffer Object with myID
	glGenBuffers(1, &vertexBufferID);
	// 必须要选一个Buffer类型绑定，ELEMENT_ARRAY OR ARRAY
	//（或许可以理解成Buffer object连接上ARRAY）
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	// Send Data to Buffer(哪个buff，data大小，data, 送到哪里）
	glBufferData(GL_ARRAY_BUFFER, MAX_TRIS*TRIANGLE_BYTE_SIZE, NULL, GL_STATIC_DRAW);
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
	GLushort indices[] = { 0,1,2,3,4,5 };
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

	GLuint programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);

	if (!checkProgramStatus(programID))
		return;

	glUseProgram(programID);
}

void LeftTriangleMove(int input)
{
	if (numTris == MAX_TRIS)
		return;
	const GLfloat THIS_TRI_Y = 1 - numTris * Y_DELTA;
	GLfloat thisTri[] =
	{
		-1.0, THIS_TRI_Y, 0.0f,
		1.0f, 0.0f, 0.0f,

		-0.9f, THIS_TRI_Y - Y_DELTA/2, 0.0f,
		1.0f, 0.0f, 0.0f,

		-1.0f, THIS_TRI_Y - Y_DELTA, 0.0f,
		1.0f, 0.0f, 0.0f
	};
	glBufferSubData(GL_ARRAY_BUFFER,  numTris * TRIANGLE_BYTE_SIZE, TRIANGLE_BYTE_SIZE, thisTri);
	if (THIS_TRI_Y - Y_DELTA < -1.1 && input > 0.0)
	{
		input = 0;
	}
	else if (THIS_TRI_Y > 0.8 && input < 0.0)
	{
		input = 0;
	}
	numTris = numTris + input;
}

void RightTriangleMove(int input)
{
	if (numTris == MAX_TRIS)
		return;
	const GLfloat THIS_TRI_Y = 1 - numTris * Y_DELTA;
	GLfloat thisTri[] =
	{
		1.0, THIS_TRI_Y, 0.0f,
		0.0f, 1.0f, 0.0f,

		0.9f, THIS_TRI_Y - Y_DELTA / 2, 0.0f,
		0.0f, 1.0f, 0.0f,

		1.0f, THIS_TRI_Y - Y_DELTA, 0.0f,
		0.0f, 1.0f, 0.0f
	};
	glBufferSubData(GL_ARRAY_BUFFER, numTris * TRIANGLE_BYTE_SIZE, TRIANGLE_BYTE_SIZE, thisTri);
	if (THIS_TRI_Y - Y_DELTA < -1.1 && input > 0.0)
	{
		input = 0;
	}
	else if (THIS_TRI_Y > 0.8 && input < 0.0)
	{
		input = 0;
	}
	numTris = numTris + input;
}


void MyGLWindow::initializeGL()
{
	// Init glew
	glewInit();

	glEnable(GL_DEPTH_TEST);

	sendDataToOpenGL();

	installShaders();


}

void MyGLWindow::keyPressEvent(QKeyEvent *e)
{
	switch (e->key())
	{
		//F1键为全屏和普通屏显示切换键
	case Qt::Key_W: 
		LeftTriangleMove(-1);
		break;
	case Qt::Key_S: 
		LeftTriangleMove(1);
		break;
	case Qt::Key_Up:
		RightTriangleMove(-1);
		break;
	case Qt::Key_Down:
		RightTriangleMove(1);
		break;
	case Qt::Key_Escape:
		close();
	}
}

void MyGLWindow::paintGL()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	// Set Viewport Width, Height
	glViewport(0, 0, width(), height());
	
	// 开始绘制（绘制类型，第一个数据，多少个vertex渲染）
	glDrawArrays(GL_TRIANGLES, (numTris-1)*NUM_VERTICES_PER_TRI, NUM_VERTICES_PER_TRI);
	// Draw ELEMENT ARRAY
	//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);



	update();
}

