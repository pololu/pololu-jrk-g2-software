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

  menu_bar = new QMenuBar();

  options_menu = menu_bar->addMenu(tr("Options"));

  save_settings_action = new QAction(this);
  save_settings_action->setText("Save settings");
  save_settings_action->setShortcut(Qt::CTRL + Qt::Key_S);

  load_settings_action = new QAction(this);
  load_settings_action->setText("Load settings");
  load_settings_action->setShortcut(Qt::CTRL + Qt::Key_L);

  dark_theme_action = new QAction(this);
  dark_theme_action->setText(tr("&Use dark theme"));

  default_theme_action = new QAction(this);
  default_theme_action->setText(tr("&Use default theme"));

  options_menu->addAction(save_settings_action);
  options_menu->addAction(load_settings_action);
  options_menu->addAction(dark_theme_action);

  connect(save_settings_action, &QAction::triggered, this,
    &graph_window::save_settings);

  connect(load_settings_action, &QAction::triggered, this,
    &graph_window::load_settings);

  connect(dark_theme_action, &QAction::triggered, this,
    &graph_window::switch_to_dark);

  connect(default_theme_action, &QAction::triggered, this,
      &graph_window::switch_to_default);

  central_layout->setMenuBar(menu_bar);

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
}

// Shows the window and directs focus to it.
//
// We use "showNormal" instead of "show" to fix a Qt bug that happens when the
// window is maximized, closed, and then reopened.  The bug causes the window be
// maximized while only painting controls in its upper left corner.
void graph_window::raise_window()
{
  if (!isVisible())
  {
    showNormal();
  }

  raise();

  activateWindow();
}

void graph_window::save_settings()
{
  QString filename = QFileDialog::getSaveFileName((QWidget* )0,
    "Save graph settings", QString(), "*.txt", Q_NULLPTR);

  if (filename.isEmpty())
  {
    return;
  }

  if (QFileInfo(filename).suffix().isEmpty())
  {
    filename.append(".txt");
  }

  QFile file_out(filename);
  if (file_out.open(QFile::WriteOnly | QFile::Text)) {
    QTextStream out(&file_out);
    for(auto plot : grabbed_widget->all_plots)
    {
      out << plot->display->text() << "," << plot->display->isChecked() << "," <<
        plot->position->cleanText() << "," << plot->scale->cleanText() << ","
        << plot->default_color << "," << plot->dark_color << '\n';
    }
  } else {
    return;
  }
  file_out.close();
}

void graph_window::load_settings()
{
  QStringList all_plots_settings;

  QString filename = QFileDialog::getOpenFileName((QWidget* )0,
    "Load graph settings", QString(), "*.txt");

  QFile file_in(filename);
  if (file_in.open(QFile::ReadOnly | QFile::Text)) {
    QTextStream stream_in(&file_in);
    while (!stream_in.atEnd())
      all_plots_settings += stream_in.readLine();
  } else {
    return;
  }

  for (int i = 0; i < all_plots_settings.size(); i++)
  {
    QStringList settings = all_plots_settings[i].split(",");

    if (settings[3].toDouble() < 0.1)
    {
      settings[3] = "0.1";
    }

    grabbed_widget->all_plots[i]->display->setChecked(settings[1].toInt());
    double lower_range = -(settings[3].toDouble() * 5.0) - (settings[2].toDouble());
    double upper_range = (settings[3].toDouble() * 5.0) - (settings[2].toDouble());
    grabbed_widget->all_plots[i]->axis->setRange(lower_range, upper_range);
    grabbed_widget->all_plots[i]->default_color = settings[4];
    grabbed_widget->all_plots[i]->dark_color = settings[5];
    if (grabbed_widget->dark_theme)
    {
      switch_to_dark();
    }
    else
      switch_to_default();
  }
}

void graph_window::switch_to_dark()
{
  for (auto plot : grabbed_widget->all_plots)
  {
    plot->display->setStyleSheet("border: 2px solid "+ plot->dark_color + ";"
      "padding: 2px;"
      "background-color: white;");
    plot->graph->setPen(QPen(plot->dark_color));
    plot->axis_label->setColor(plot->dark_color);
    plot->axis_position_label->setColor(plot->dark_color);
    plot->axis_scale_label->setColor(plot->dark_color);
    for (auto label : plot->axis_top_and_bottom)
      label->setColor(plot->dark_color);
  }

  QLinearGradient axis_rect_gradient;
  axis_rect_gradient.setStart(0, 0);
  axis_rect_gradient.setFinalStop(0, 350);
  axis_rect_gradient.setColorAt(0, QColor(40, 40, 40));
  axis_rect_gradient.setColorAt(1, QColor(10, 10, 10));
  grabbed_widget->custom_plot->axisRect()->setBackground(axis_rect_gradient);
  grabbed_widget->custom_plot->setBackground(QColor(170, 170, 170));
  grabbed_widget->custom_plot->xAxis->grid()->setPen(QPen(QColor(200, 200, 200, 140), 0, Qt::SolidLine));
  grabbed_widget->custom_plot->yAxis->grid()->setPen(QPen(QColor(200, 200, 200, 140), 0, Qt::SolidLine));
  grabbed_widget->custom_plot->xAxis->setBasePen(QPen(QColor(Qt::white), 1, Qt::SolidLine));
  grabbed_widget->custom_plot->yAxis->setBasePen(QPen(QColor(Qt::white), 1, Qt::SolidLine));
  grabbed_widget->custom_plot->yAxis->grid()->setZeroLinePen(QPen(QColor(200, 200, 200, 140), 0, Qt::SolidLine));

  options_menu->removeAction(dark_theme_action);
  options_menu->addAction(default_theme_action);

  grabbed_widget->dark_theme = true;

  grabbed_widget->custom_plot->replot();
}

void graph_window::switch_to_default()
{
  for (auto plot : grabbed_widget->all_plots)
  {
    plot->display->setStyleSheet("border: 2px solid "+ plot->default_color + ";"
      "padding: 2px;"
      "background-color: white;");
    plot->graph->setPen(QPen(plot->default_color));
    plot->axis_label->setColor(plot->default_color);
    plot->axis_position_label->setColor(plot->default_color);
    plot->axis_scale_label->setColor(plot->default_color);
    for (auto label : plot->axis_top_and_bottom)
      label->setColor(plot->default_color);
  }

  grabbed_widget->custom_plot->axisRect()->setBackground(QColor(Qt::white));
  grabbed_widget->custom_plot->setBackground(QColor(Qt::white));
  grabbed_widget->custom_plot->xAxis->grid()->setPen(QPen(QColor(100, 100, 100, 140), 0, Qt::SolidLine));
  grabbed_widget->custom_plot->yAxis->grid()->setPen(QPen(QColor(100, 100, 100, 140), 0, Qt::SolidLine));
  grabbed_widget->custom_plot->xAxis->setBasePen(QPen(QColor(Qt::black), 1, Qt::SolidLine));
  grabbed_widget->custom_plot->yAxis->setBasePen(QPen(QColor(Qt::black), 1, Qt::SolidLine));
  grabbed_widget->custom_plot->yAxis->grid()->setZeroLinePen(QPen(QColor(100, 100, 100, 140), 0, Qt::SolidLine));

  options_menu->removeAction(default_theme_action);
  options_menu->addAction(dark_theme_action);

  grabbed_widget->dark_theme = false;

  grabbed_widget->custom_plot->replot();
}
