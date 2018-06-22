#include "target_slider.h"

#include <QPainter>
#include <QPaintEvent>
#include <QStyleOptionSlider>

void target_slider::paintEvent(QPaintEvent * event)
{
  QSlider::paintEvent(event);

  int ball_value = 2100;
  QColor ball_color;

  if (ball_value >= minimum() && ball_value <= maximum())
  {
    QStyleOptionSlider opt;
    initStyleOption(&opt);

    QRect groove_rect = style()->subControlRect(QStyle::CC_Slider, &opt,
      QStyle::SC_SliderGroove, this);
    QRect slider_rect = style()->subControlRect(QStyle::CC_Slider, &opt,
      QStyle::SC_SliderHandle, this);

    int span, length_offset, thickness_offset, radius;
    QPainter painter(this);

    if (orientation() == Qt::Horizontal)
    {
      span = groove_rect.width() - slider_rect.width();
      length_offset = groove_rect.x() + slider_rect.width() / 2;
      thickness_offset = height() / 2;
      radius = height() / 4;
    }
    else
    {
      span = groove_rect.height() - slider_rect.height();
      length_offset = groove_rect.y() + slider_rect.height() / 2;
      thickness_offset = width() / 2;
      radius = width() / 4;

      painter.rotate(90);
      painter.scale(1, -1);
    }
    int pos = QStyle::sliderPositionFromValue(minimum(), maximum(),
      ball_value, span);

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(ball_color);
    painter.translate(length_offset, thickness_offset);
    painter.drawEllipse(QPoint(pos, 0), radius, radius);
  }

  // Restore the saved page step value.
  //setPageStep(ps);
}
