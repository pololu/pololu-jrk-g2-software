#include "target_slider.h"

#include <QApplication>
#include <QPainter>
#include <QPaintEvent>
#include <QStyleOptionSlider>

#include <assert.h>

target_slider::target_slider()
{
  fusion_style = QApplication::style()->objectName() == "fusion";
  setOrientation(Qt::Horizontal);
}

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

  int radius;
  if (fusion_style)
  {
    // Make sure the dot it not too big.
    radius = handle_rect.width() / 4 + 1;
  }
  else
  {
    radius = handle_rect.width() / 2 - 1;
  }

  int thickness_offset = height() / 2 - radius;
  int pos = QStyle::sliderPositionFromValue(minimum(), maximum(), value, span);

  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  if (fusion_style)
  {
    // Make the dark green dot more visible against the blue groove background.
    painter.setPen(QPen(QColor(Qt::white)));
  }
  else
  {
    painter.setPen(Qt::NoPen);
  }

  painter.setBrush(color);
  painter.translate(length_offset, thickness_offset);
  painter.drawEllipse(QPoint(pos, 0), radius, radius);
}
