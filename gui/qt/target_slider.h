#include <QSlider>

class target_slider : public QSlider
{
  Q_OBJECT;

public:
  target_slider(Qt::Orientation o) : QSlider(o) { }

protected:
  void paintEvent(QPaintEvent * event) override;
};
