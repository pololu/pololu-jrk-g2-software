#include <QSlider>

class target_slider : public QSlider
{
  Q_OBJECT;

public:
  using QSlider::QSlider;

  void set_scaled_feedback_enabled(bool enabled)
  {
    scaled_feedback.enabled = enabled;
    update();
  }

  void set_scaled_feedback(int value)
  {
    scaled_feedback.value = value;
    update();
  }

  void set_duty_cycle_enabled(bool enabled)
  {
    duty_cycle.enabled = enabled;
    update();
  }

  void set_duty_cycle(int value)
  {
    duty_cycle.value = value;
    update();
  }

protected:
  void paintEvent(QPaintEvent * event) override;
  void draw_ball(int value, const QColor & color);

  struct ball
  {
    bool enabled = false;
    int value = 0;
  } scaled_feedback, duty_cycle;
};
