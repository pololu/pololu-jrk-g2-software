#include <QApplication>
#include <QtCore>

#include "qcustomplot.h"
#include "main_window.h"

int main(int argc, char ** argv)
{
	//QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	// qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "0");

	QApplication app(argc, argv);
  	GraphWindow window;
  	window.setWindowTitle("jrk Graph");
  	window.resize(QSize(818,547));
	window.show();
	
  	return app.exec();
}