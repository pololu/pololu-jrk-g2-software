#include <QApplication>
#include <QtCore>
#include "qcustomplot.h"
#include "graphwindow.h"
#include "mainwindow.h"

int main(int argc, char ** argv)
{
	//QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	//qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "1");

	QApplication app(argc, argv);
  	MainWindow window;
  	// GraphWindow window2;
	window.show();
	// window2.show();
  	return app.exec();
}