#pragma once
#include <QtOpenGL\qgl>

class MyGLWindow : public QGLWidget
{
protected:
	
	void sendDataToOpenGL();
	void initializeGL();
	void StartGame();
	void keyPressEvent(QKeyEvent * e);
	void paintGL();
	void Draw();
public:
	MyGLWindow();
	void Update(float TriTranslate[2]);
	
};

