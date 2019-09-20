#pragma once
#include <QtOpenGL\qgl>

class MyGLWindow : public QGLWidget
{
protected:
	
	void sendDataToOpenGL();
	bool checkStatus(GLuint objectID, PFNGLGETSHADERIVPROC objectPropertyGetter, PFNGLGETSHADERINFOLOGPROC getInfoLogFunc, GLenum statusType);
	bool checkShaderStatus(GLuint shaderID);
	bool checkProgramStatus(GLuint programID);
	void initializeGL();
	void StartGame();
	void keyPressEvent(QKeyEvent * e);
	void randomColor(float * color);
	void installShaders();
	void paintGL();
	void Draw();
public:
	MyGLWindow();
	~MyGLWindow();
	void Update(float TriTranslate[2], float randcolor[3]);
	
};

