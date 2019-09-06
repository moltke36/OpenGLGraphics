#pragma once
#include <QtOpenGL\qgl>

class MyGLWindow : public QGLWidget
{
protected:
	void sendDataToOpenGL();
	void initializeGL();
	void paintGL();

public:

};

