#include <GL\glew.h>
#include "MyGLWindow.h"

extern const char* vertexShaderCode;
extern const char* fragmentShaderCode;

void MyGLWindow::sendDataToOpenGL()
{

	// Create verts
	GLfloat verts[] =
	{
		+0.0f,+0.0f,
		+1.0f, +0.0f, +0.0f,
		+1.0f,+1.0f,
		+1.0f, +0.0f, +0.0f,
		-1.0f,+1.0f,
		+1.0f, +0.0f, +0.0f,
		-1.0f,-1.0f,
		+1.0f, +0.0f, +0.0f,
		+1.0f,-1.0f,
		+1.0f, +0.0f, +0.0f
	};

	// Create a BufferID
	GLuint vertexBufferID;
	// Create a Buffer Object with myID
	glGenBuffers(1, &vertexBufferID);
	// ����Ҫѡһ��Buffer���Ͱ󶨣�ELEMENT_ARRAY OR ARRAY
	//�������������Buffer object������ARRAY��
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	// Send Data to Buffer(�ĸ�buff��data��С��data, �͵����
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
	// Enable Vertex Attribute (vertex�кܶ಻ͬ���ԣ�λ����һ��);
	glEnableVertexAttribArray(0);
	// ����Vertex���ݣ�λ�ã���������һ���㣬GLFloat���ͣ�����Normalize��֮����ͣ�֮����ͣ�
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
	// Enable Second Attribute of the vertex
	glEnableVertexAttribArray(1);
	// tell openGL how to interpret data
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (char*)(sizeof(float) * 2));

	// ELEMENT ARRAY
	// Define indices
	GLushort indices[] = { 0,1,2,0,3,4 };
	// Create a index buffer ID
	GLuint indexBufferID;
	// Create a index buffer
	glGenBuffers(1, &indexBufferID);
	// Bind to ELEMENT ARRAY BUFFER
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
	// Pass data to ELEMENT ARRAY
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

void installShaders()
{
	// Create ID for Shaders
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	
	// Create a char pointer to read the shader string
	const char* adapter[1];
	adapter[0] = vertexShaderCode;
	// 0 means auto calulate when the compiler see a /0 null terminator
	glShaderSource(vertexShaderID, 1, adapter, 0);
	adapter[0] = fragmentShaderCode;
	glShaderSource(fragmentShaderID, 1, adapter, 0);

	glCompileShader(vertexShaderID);
	glCompileShader(fragmentShaderID);

	GLuint programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);

	glUseProgram(programID);
}

void MyGLWindow::initializeGL()
{
	// Init glew
	glewInit();

	sendDataToOpenGL();

	installShaders();
}

void MyGLWindow::paintGL()
{
	// Set Viewport Width, Height
	glViewport(0, 0, width(), height());
	// ��ʼ���ƣ��������ͣ���һ�����ݣ����ٸ�vertex��Ⱦ��
	//glDrawArrays(GL_TRIANGLES, 0, 40);
	// Draw ELEMENT ARRAY
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
	//glClear(GL_COLOR_BUFFER_BIT);
}
