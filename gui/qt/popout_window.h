#pragma once

#include <QWidget>

class QLayout;
class QMainWindow;

class popout_window : public QWidget
{
  Q_OBJECT

  QLayout * later_layout;

public:
  popout_window();
  void set_later_layout(QLayout * layout);

public slots:
  void prepare_and_activate();
};
