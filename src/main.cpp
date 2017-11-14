#include <QApplication>
#include <QtCore>
#include "qcustomplot.h"
#include "main_window.h"

int main(int argc, char ** argv)
{
	//QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	//qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "1");

	QApplication app(argc, argv);
  	MainWindow window;
	window.show();
  	return app.exec();
}