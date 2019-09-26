#pragma once
#include <QtOpenGL\qgl>
#include <glm.hpp>


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
	void UpdateUniform(glm::vec2 TriTranslate, float randcolor[3]);

	void DetectCollision();
	
};

