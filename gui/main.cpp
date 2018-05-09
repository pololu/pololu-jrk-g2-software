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
  QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

#ifdef _WIN32
  SetProcessDPIAware();
#endif

  QApplication app(argc, argv);
  main_controller controller;
  main_window window;
  controller.set_window(&window);
  window.set_controller(&controller);

  window.show();
  return app.exec();
}
