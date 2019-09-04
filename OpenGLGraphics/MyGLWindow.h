#pragma once
#include <QtOpenGL\qgl>

class MyGLWindow : public QGLWidget
{
protected:
	void initializeGL();
	void paintGL();

public:

};

