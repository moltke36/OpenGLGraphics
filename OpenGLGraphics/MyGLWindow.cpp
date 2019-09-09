#include <GL\glew.h>
#include <iostream>
#include <fstream>
#include "MyGLWindow.h"


void MyGLWindow::sendDataToOpenGL()
{
	const float RED_TRIANGLE_Z = 0.5f;
	const float BLUE_TRIANBLE_Z = -0.5f;

	// Create verts
	GLfloat verts[] =
	{
		-1.0f,-1.0f, RED_TRIANGLE_Z,
		+1.0f, +0.0f, +0.0f,
		+0.0f,+1.0f, -1.0f,
		+1.0f, +1.0f, +1.0f,
		+1.0f,-1.0f, RED_TRIANGLE_Z,
		+1.0f, +0.0f, +0.0f,

		-1.0f,+1.0f,BLUE_TRIANBLE_Z,
		+0.0f, +0.0f, +1.0f,
		+0.0f,-1.0f,BLUE_TRIANBLE_Z,
		+0.0f, +0.0f, +1.0f,
		+1.0f,+1.0f,BLUE_TRIANBLE_Z,
		+0.0f, +0.0f, +1.0f
	};

	// Create a BufferID
	GLuint vertexBufferID;
	// Create a Buffer Object with myID
	glGenBuffers(1, &vertexBufferID);
	// 必须要选一个Buffer类型绑定，ELEMENT_ARRAY OR ARRAY
	//（或许可以理解成Buffer object连接上ARRAY）
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	// Send Data to Buffer(哪个buff，data大小，data, 送到哪里）
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
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


void MyGLWindow::initializeGL()
{
	// Init glew
	glewInit();

	glEnable(GL_DEPTH_TEST);

	sendDataToOpenGL();

	installShaders();
}

void MyGLWindow::paintGL()
{
	glClear(GL_DEPTH_BUFFER_BIT);
	// Set Viewport Width, Height
	glViewport(0, 0, width(), height());
	// 开始绘制（绘制类型，第一个数据，多少个vertex渲染）
	//glDrawArrays(GL_TRIANGLES, 0, 40);
	// Draw ELEMENT ARRAY
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
	//glClear(GL_COLOR_BUFFER_BIT);
}
