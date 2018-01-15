#include <QApplication>
#include <QtCore>
#include "main_controller.h"
#include "main_window.h"

int main(int argc, char ** argv)
{
#if QT_VERSION >= QT_VERSION_CHECK(5,6,0)
  QApplication::setAttribute(Qt::AA_DisableHighDpiScaling);
#else
  qputenv("QT_DEVICE_PIXEL_RATIO", QByteArray("1"));
#endif // QT_VERSION


  QApplication app(argc, argv);
  main_controller controller;
  main_window window;
  controller.set_window(&window);
  window.set_controller(&controller);
  window.show();
  return app.exec();
}
