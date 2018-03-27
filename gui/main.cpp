#include <QGuiApplication>
#include <QtCore>
#include <QDesktopWidget>
#include "main_controller.h"
#include "main_window.h"

#ifdef _WIN32
#include <windows.h>
#endif

int main(int argc, char ** argv)
{
  // AA_EnableHighDpiScaling was added in Qt 5.6.
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
  QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

#ifdef _WIN32
  SetProcessDPIAware();
#endif

  QApplication app(argc, argv);
  main_controller controller;
  main_window window;
  controller.set_window(&window);
  window.set_controller(&controller);


  // makes application open intially in center of primary display
  QRect screenGeometry = QApplication::desktop()->screenGeometry();
  int x = (screenGeometry.width()-window.width()) / 2;
  int y = (screenGeometry.height()-window.height()) / 2;
  window.move(x, y);
  window.show();
  return app.exec();
}
