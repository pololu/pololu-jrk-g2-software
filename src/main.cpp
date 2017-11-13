#include <QApplication>
#include <QtCore>

#include "qcustomplot.h"
#include "main_window.h"

int main(int argc, char ** argv)
{
	//QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	// qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "0");

	QApplication app(argc, argv);
  	OtherWindow window;
  	window.setWindowTitle("jrk Graph");
  	window.resize(QSize(818,547));
	window.show();
	// QTimer newTimer;
	QObject::connect(&window.dataTimer, SIGNAL(timeout()), &window, SLOT(realtimeDataSlot()));
    window.dataTimer.start(30); // Interval 0 means to refresh as fast as possible
  	return app.exec();
}