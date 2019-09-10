#pragma once
#include <QtOpenGL\qgl>

class MyGLWindow : public QGLWidget
{
protected:
	void sendDataToOpenGL();
	void MoveTriangle(float left, float right);
	void initializeGL();
	void keyPressEvent(QKeyEvent * e);
	void paintGL();

public:

};

