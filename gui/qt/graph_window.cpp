#include "graph_window.h"
#include "main_window.h"
#include "graph_widget.h"

graph_window::graph_window(QWidget *parent)
{
  setup_ui();
  grabbed_widget = 0;
}

void graph_window::setup_ui()
{
  setObjectName(QStringLiteral("graph_window"));

  setWindowTitle(tr("graph_window",
    "Pololu Jrk G2 Configuration Utility - Plots of Variables vs. Time"));

  central_layout = new QGridLayout();

  setLayout(central_layout);

  QMetaObject::connectSlotsByName(this);
}

void graph_window::closeEvent(QCloseEvent *event)
{
  central_layout->removeWidget(grabbed_widget);
  emit pass_widget(grabbed_widget);
  grabbed_widget = 0;
  setWindowState(Qt::WindowNoState);
  QWidget::closeEvent(event);
}

void graph_window::receive_widget(graph_widget *widget)
{

  grabbed_widget = widget;
  grabbed_widget->set_preview_mode(false);

  central_layout->addWidget(grabbed_widget->custom_plot,
    0, 0, 14, 17);
  central_layout->addLayout(grabbed_widget->bottom_control_layout,
    15, 0, 1, 1, Qt::AlignLeft);
  central_layout->addLayout(grabbed_widget->plot_visible_layout,
    0, 18, 11, 1, Qt::AlignTop | Qt::AlignRight);
}
