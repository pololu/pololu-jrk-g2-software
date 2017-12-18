#include <QApplication>
#include <QtCore>
#include "main_controller.h"
#include "main_window.h"
#include <iostream>  // tmphax

int main(int argc, char ** argv)
{
  // QApplication::setAttribute(Qt::AA_DisableHighDpiScaling);
  // qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "0");

  // SetProcessDPIAware();

  std::cout << "main start" << std::endl;  // tmphax

  QApplication app(argc, argv);
  main_controller controller;
  main_window window;
  controller.set_window(&window);
  window.set_controller(&controller);
  window.show();
  return app.exec();
}
