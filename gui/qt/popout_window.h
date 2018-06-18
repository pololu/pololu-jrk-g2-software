#include <QWidget>

class QLayout;
class QMainWindow;

class popout_window : public QWidget
{
  QLayout * later_layout;

public:
  void set_later_layout(QLayout * layout);

public slots:
  void prepare_and_activate();
};
