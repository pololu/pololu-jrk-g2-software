#include "graph_window.h"
#include "main_window.h"
#include "graph_widget.h"

#include <iostream>

graph_window::graph_window(QWidget *parent)
{
  setup_ui();
  grabbed_widget = 0;
}

void graph_window::setup_ui()
{
  setObjectName("graph_window");

  setWindowTitle(tr("Pololu Jrk G2 Configuration Utility - Graph"));

  central_layout = new QGridLayout();

  setLayout(central_layout);

  QMetaObject::connectSlotsByName(this);
}

void graph_window::closeEvent(QCloseEvent *event)
{
  central_layout->removeWidget(grabbed_widget);
  emit pass_widget(grabbed_widget);
  grabbed_widget = 0;

  QWidget::closeEvent(event);
}

void graph_window::receive_widget(graph_widget *widget)
{
  grabbed_widget = widget;

  central_layout->addWidget(grabbed_widget->custom_plot,
    0, 0, 2, 2);
  central_layout->addLayout(grabbed_widget->bottom_control_layout,
    2, 0, Qt::AlignLeft);
  central_layout->addLayout(grabbed_widget->plot_visible_layout,
    0, 2, Qt::AlignTop);

   central_layout->setColumnStretch(1, 3);
   central_layout->setRowStretch(1, 3);

   grabbed_widget->set_preview_mode(false);
}

// Directs focus to graph_window.
void graph_window::raise_window()
{
  // Shows graph_window if not already shown.
  // Reverts graph_window state to normal to fix Qt bug when window is
  // maximized, closed, and then reopened.
  if (!this->isVisible())
    this->showNormal();

  this->raise();

  this->activateWindow();
}
