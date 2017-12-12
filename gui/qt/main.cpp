#include <QApplication>
#include <QtCore>
#include "qcustomplot.h"
#include "graphwindow.h"
#include "mainwindow.h"

int main(int argc, char ** argv)
{
  // QApplication::setAttribute(Qt::AA_DisableHighDpiScaling);
	// qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "0");

	// SetProcessDPIAware();

	QApplication app(argc, argv);
  	main_window window;
	window.show();
  	return app.exec();
}