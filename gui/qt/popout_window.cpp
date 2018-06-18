#include "popout_window.h"

#include <QMainWindow>

popout_window::popout_window()
{
  // We want this to be the same background color as a tab widget so that
  // scroll bars look good on it.
  setStyleSheet(".popout_window { background-color: white; }");
}

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

void popout_window::closeEvent(QCloseEvent * event)
{
  Q_UNUSED(event);
  emit window_closed();
}
