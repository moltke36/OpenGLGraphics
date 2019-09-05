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
	// 必须要选一个Buffer类型绑定，ELEMENT_ARRAY OR ARRAY
	//（或许可以理解成Buffer object连接上ARRAY）
	glBindBuffer(GL_ARRAY_BUFFER, myBufferID);
	// Send Data to Buffer(哪个buff，data大小，data, 送到哪里）
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
	// Enable Vertex Attribute (vertex有很多不同属性，位置是一个);
	glEnableVertexAttribArray(0);
	// 解释Vertex数据（位置，两个数据一个点，GLFloat类型，禁用Normalize，之后解释，之后解释）
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
}

void MyGLWindow::paintGL()
{
	// Set Viewport Width, Height
	glViewport(0, 0, width(), height());
	// 开始绘制（绘制类型，第一个数据，多少个vertex渲染）
	glDrawArrays(GL_TRIANGLES, 0, 40);
	//glClear(GL_COLOR_BUFFER_BIT);
}
