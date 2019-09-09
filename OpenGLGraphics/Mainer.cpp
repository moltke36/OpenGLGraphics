#include <QtWidgets/qapplication.h>
#include <MyGLWindow.h>

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);
	MyGLWindow mywindow;
	mywindow.resize(640, 480);
	mywindow.show();
	return app.exec();
}
