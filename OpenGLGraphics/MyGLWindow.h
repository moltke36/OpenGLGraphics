#pragma once
#include <QtOpenGL\qgl>

class MyGLWindow : public QGLWidget
{
private:
	
	void sendDataToOpenGL();
	bool checkStatus(GLuint objectID, PFNGLGETSHADERIVPROC objectPropertyGetter, PFNGLGETSHADERINFOLOGPROC getInfoLogFunc, GLenum statusType);
	bool checkShaderStatus(GLuint shaderID);
	bool checkProgramStatus(GLuint programID);
	void initializeGL();
	void StartGame();
	void keyPressEvent(QKeyEvent * e);

protected:
	void installShaders();
	void paintGL();
	void Draw();
	void mouseMoveEvent(QMouseEvent*);

public:
	MyGLWindow();
	~MyGLWindow();
	void Update();
	
};

