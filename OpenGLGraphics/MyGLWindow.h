#pragma once
#include <QtOpenGL\qgl>

class MyGLWindow : public QGLWidget
{
protected:
	
	void sendDataToOpenGL();
	void initializeGL();
	void StartGame();
	void keyPressEvent(QKeyEvent * e);
	void randomColor(float * color);
	void paintGL();
	void DrawDynamic();
	void DrawStaic();
public:
	MyGLWindow();
	void UpdateUniform(float TriTranslate[2], float randcolor[3]);
	
};

