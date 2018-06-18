#include "popout_window.h"

#include <QMainWindow>

void popout_window::set_later_layout(QLayout * layout)
{
  later_layout = layout;
}

void popout_window::prepare_and_activate()
{
  setLayout(later_layout);

  if (!isVisible() || (windowState() & Qt::WindowMinimized))
  {
    showNormal();
  }
  raise();
  activateWindow();
}
