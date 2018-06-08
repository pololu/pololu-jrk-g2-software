#include <QProcessEnvironment>
#include <QStyleFactory>
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

  // On non-Windows systems, use Qt's fusion style instead of a
  // native style.
#ifndef _WIN32
  QApplication::setStyle(QStyleFactory::create("fusion"));
#endif

  QApplication app(argc, argv);

  // An easy way to support systems with small screens.
  auto env = QProcessEnvironment::systemEnvironment();
  int size = env.value("JRK2GUI_FONT_SIZE").toInt();
  if (size > 0)
  {
    QFont font = app.font();
    font.setPointSize(size);
    app.setFont(font);
  }

  main_controller controller;
  main_window window;
  controller.set_window(&window);
  window.set_controller(&controller);

  window.show();
  return app.exec();
}
