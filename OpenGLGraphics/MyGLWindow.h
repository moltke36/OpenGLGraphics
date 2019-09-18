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
	void Draw();
public:
	MyGLWindow();
	~MyGLWindow();
	void Update(float TriTranslate[2], float randcolor[3]);
	
};

