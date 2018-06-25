#include "target_slider.h"

#include <QPainter>
#include <QPaintEvent>
#include <QStyleOptionSlider>

#include <assert.h>

void target_slider::paintEvent(QPaintEvent * event)
{
  assert(orientation() == Qt::Horizontal);
  QSlider::paintEvent(event);

  if (isEnabled() && scaled_feedback.enabled)
  {
    draw_ball(scaled_feedback.value, "#ff0000");
  }

  if (isEnabled() && duty_cycle.enabled)
  {
    draw_ball(duty_cycle.value + 2048, "#006400");
  }
}

void target_slider::draw_ball(int value, const QColor & color)
{
  if (value < minimum() || value > maximum()) { return; }

  QStyleOptionSlider opt;
  initStyleOption(&opt);

  QRect groove_rect = style()->subControlRect(QStyle::CC_Slider, &opt,
    QStyle::SC_SliderGroove, this);
  QRect handle_rect = style()->subControlRect(QStyle::CC_Slider, &opt,
    QStyle::SC_SliderHandle, this);

  int span = groove_rect.width() - handle_rect.width();
  int length_offset = groove_rect.x() + handle_rect.width() / 2;
  int radius = handle_rect.width() / 2 - 1;
  int thickness_offset = height() / 2 - radius;
  int pos = QStyle::sliderPositionFromValue(minimum(), maximum(), value, span);

  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);
  painter.setPen(Qt::NoPen);
  painter.setBrush(color);
  painter.translate(length_offset, thickness_offset);
  painter.drawEllipse(QPoint(pos, 0), radius, radius);
}
