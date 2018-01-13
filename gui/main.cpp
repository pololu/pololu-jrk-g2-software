#include <QApplication>
#include <QtCore>
#include "main_controller.h"
#include "main_window.h"

int main(int argc, char ** argv)
{
  // QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "1");

  // SetProcessDPIAware();

  QApplication app(argc, argv);
  main_controller controller;
  main_window window;
  controller.set_window(&window);
  window.set_controller(&controller);
  window.show();
  return app.exec();
}
