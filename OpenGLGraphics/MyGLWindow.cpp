#include <GL\glew.h>
#include "MyGLWindow.h"

void MyGLWindow::initializeGL()
{
	// Init glew
	glewInit();

	// Create verts
	GLfloat verts[] =
	{
		+0.0f,+0.0f,
		+1.0f,+1.0f,
		+0.9f,+1.0f,

		+0.9f,+1.0f,
		+0.0f,+0.0f,
		+0.0f,+0.1f,

		+0.0f,+0.0f,
		-1.0f,+1.0f,
		-0.9f,+1.0f,

		-0.9f,+1.0f,
		+0.0f,+0.0f,
		+0.0f,+0.1f,


		+0.0f,+0.0f,
		+1.0f,-1.0f,
		+0.9f,-1.0f,

		+0.9f,-1.0f,
		+0.0f,+0.0f,
		+0.0f,-0.1f,

		+0.0f,+0.0f,
		-1.0f,-1.0f,
		-0.9f,-1.0f,

		-0.9f,-1.0f,
		+0.0f,+0.0f,
		+0.0f,-0.1f,
		

		-1.0f,+0.9f,
		-0.1f,+0.0f,
		-1.0f,-0.9f,

		+1.0f,+0.9f,
		+0.1f,+0.0f,
		+1.0f,-0.9f
	};

	// Create a BufferID
	GLuint myBufferID;
	// Create a Buffer Object with myID
	glGenBuffers(1, &myBufferID);
	// ����Ҫѡһ��Buffer���Ͱ󶨣�ELEMENT_ARRAY OR ARRAY
	//�������������Buffer object������ARRAY��
	glBindBuffer(GL_ARRAY_BUFFER, myBufferID);
	// Send Data to Buffer(�ĸ�buff��data��С��data, �͵����
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
	// Enable Vertex Attribute (vertex�кܶ಻ͬ���ԣ�λ����һ��);
	glEnableVertexAttribArray(0);
	// ����Vertex���ݣ�λ�ã���������һ���㣬GLFloat���ͣ�����Normalize��֮����ͣ�֮����ͣ�
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
}

void MyGLWindow::paintGL()
{
	// Set Viewport Width, Height
	glViewport(0, 0, width(), height());
	// ��ʼ���ƣ��������ͣ���һ�����ݣ����ٸ�vertex��Ⱦ��
	glDrawArrays(GL_TRIANGLES, 0, 40);
	//glClear(GL_COLOR_BUFFER_BIT);
}
