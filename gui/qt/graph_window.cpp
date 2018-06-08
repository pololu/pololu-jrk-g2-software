#include "graph_window.h"
#include "main_window.h"
#include "graph_widget.h"

graph_window::graph_window(QWidget *parent)
{
  setup_ui();
}

void graph_window::setup_ui()
{
  setObjectName("graph_window");

  setWindowTitle(tr("Pololu Jrk G2 Configuration Utility - Graph"));

  central_layout = new QGridLayout();

  setLayout(central_layout);
}

void graph_window::closeEvent(QCloseEvent *event)
{
  central_layout->removeWidget(grabbed_widget);

  emit pass_widget();

  QWidget::closeEvent(event);
}

void graph_window::receive_widget(graph_widget *widget)
{
  grabbed_widget = widget;

  grabbed_widget->set_preview_mode(false);

  grabbed_widget->plot_visible_layout->setParent(0);

  central_layout->addWidget(grabbed_widget->custom_plot,
    1, 0, 2, 2);
  central_layout->addLayout(grabbed_widget->plot_visible_layout,
    1, 2, Qt::AlignTop | Qt::AlignRight);

  central_layout->setColumnMinimumWidth(0,
    grabbed_widget->plot_visible_layout->sizeHint().height());

  central_layout->setColumnStretch(1, 3);
  central_layout->setRowStretch(2, 3);

  central_layout->setMenuBar(grabbed_widget->setup_menu_bar());
}

// Shows the window and directs focus to it.
//
// We use "showNormal" instead of "show" to fix a Qt bug that happens when the
// window is maximized, closed, and then reopened.  The bug causes the window be
// maximized while only painting controls in its upper left corner.
void graph_window::raise_window()
{
  if (!isVisible() || (windowState() & Qt::WindowMinimized))
  {
    showNormal();
  }

  raise();

  activateWindow();
}
