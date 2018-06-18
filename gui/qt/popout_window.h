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
  void set_later_layout(QLayout *);

public slots:
  void prepare_and_activate();

signals:
  void window_closed();

protected:
  void closeEvent(QCloseEvent *) override;
};
