#include "graph_widget.h"

#include <QColorDialog>
#include <QFileDialog>
#include <QFontDatabase>
#include <QMessageBox>
#include <QWidgetAction>

#include <assert.h>

graph_widget::graph_widget()
{
  int id = QFontDatabase::addApplicationFont(":dejavu_sans");
  QString family = QFontDatabase::applicationFontFamilies(id).at(0);
  y_label_font.setFamily(family);
  x_label_font.setFamily(family);

  setup_ui();

  connect(domain, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
    this, &graph_widget::change_ranges);
}

// Changes options for the custom_plot when in preview mode.
void graph_widget::set_preview_mode(bool preview_mode)
{
  this->preview_mode = preview_mode;

  if (preview_mode)
  {
    reset_graph_interaction_axes();

    custom_plot->axisRect()->setMargins(QMargins(40, 15, 15, 15));
    custom_plot->setCursor(Qt::PointingHandCursor);
    custom_plot->setToolTip("Click to open graph window");
  }
  else
  {
    custom_plot->axisRect()->setMargins(QMargins(55, 50, 20, 50));
    custom_plot->setCursor(Qt::ArrowCursor);
    custom_plot->setToolTip("");
  }

  for (auto plot : all_plots)
  {
    if (plot->display->isChecked())
    {
      plot->axis_label->setVisible(true);
      update_plot_text_and_arrows(*plot);
    }
  }

  custom_plot->xAxis->setTickLabels(!preview_mode);
  custom_plot->xAxis->setLabel(preview_mode ? "" : "Time (ms)");

  custom_plot->replot();
}

void graph_widget::set_paused(bool paused)
{
  if (paused != graph_paused)
  {
    graph_paused = paused;
    pause_run_button->setText(graph_paused ? "R&un" : "&Pause");
    custom_plot->replot();
  }
}

void graph_widget::clear_graphs()
{
  for (auto plot : all_plots)
  {
    plot->graph->data()->clear();
  }

  custom_plot->replot();
}

void graph_widget::plot_data(uint32_t time)
{
  current_time = time;
  if (!graph_paused) { display_time = time; }

  for (auto plot : all_plots)
  {
    plot->graph->addData(time, plot->plot_value);
  }

  remove_old_data();

  if (graph_paused) { return; }

  update_x_axis();

  for (auto plot : all_plots)
  {
    if (plot->display->isChecked())
    {
      update_plot_overflow_arrows(*plot);
    }
  }

  custom_plot->replot();
}

void graph_widget::set_checkbox_style(plot * plot, const QString & color)
{
  plot->display->setStyleSheet(
    "QCheckBox{border: 2px solid " + color + ";"
    "padding: 2px;"
    "background-color: white;}");
}

void graph_widget::change_plot_colors(plot * plot, const QString & color)
{
  set_checkbox_style(plot, color);
  plot->graph->setPen(QPen(color));
  plot->axis_label->setColor(color);
  plot->axis_position_label->setColor(color);
  plot->axis_scale_label->setColor(color);
  for (auto label : plot->overflow_arrows)
  {
    label->setColor(color);
  }

  custom_plot->replot();
}

bool graph_widget::eventFilter(QObject * object, QEvent * e)
{
  if (e->type() == QEvent::MouseButtonPress)
  {
    QMouseEvent * event = static_cast<QMouseEvent *>(e);
    for (auto plot : all_plots)
    {
      if (object != plot->display)
      {
        continue;
      }
      if (event->button() == Qt::RightButton)
      {
        show_plot_menu(plot, false);
        return true;
      }
    }
  }
  return QObject::eventFilter(object, e);
}

void graph_widget::show_plot_menu(plot * plot, bool with_title)
{
  plot->display->setCheckState(Qt::Checked);

  QMenu * menu = new QMenu();

  QAction * change_color_action = new QAction(this);
  change_color_action->setText("Change color");

  QAction * reset_color_action = new QAction(this);
  reset_color_action->setText("Reset color");

  QAction * reset_range_action = new QAction(this);
  reset_range_action->setText("Reset position and scale");

  if (with_title)
  {
    QLabel * label = new QLabel(plot->display->text());
    label->setContentsMargins(10, 5, 0, 5);
    QWidgetAction * menu_title = new QWidgetAction(menu);
    menu_title->setDefaultWidget(label);
    menu->addAction(menu_title);
    menu->addSeparator();
  }

  menu->addAction(change_color_action);
  menu->addAction(reset_color_action);
  menu->addAction(reset_range_action);

  connect(change_color_action, &QAction::triggered, [=]()
  {
    pick_plot_color(plot);
  });

  connect(reset_color_action, &QAction::triggered, [=]()
  {
    if (dark_theme)
    {
      plot->dark_changed = false;
      change_plot_colors(plot, plot->original_dark_color);
    }
    else
    {
      plot->default_changed = false;
      change_plot_colors(plot, plot->original_default_color);
    }
  });

  connect(reset_range_action, &QAction::triggered, [=]
  {
    reset_plot_range(*plot);
    custom_plot->replot();
  });

  menu->popup(QCursor::pos());
}

void graph_widget::pick_plot_color(plot * plot)
{
  QColorDialog * color_dialog = new QColorDialog(custom_plot);
  color_dialog->setCurrentColor(plot->graph->pen().color());
  color_dialog->setOption(QColorDialog::DontUseNativeDialog);
  color_dialog->move(QCursor::pos());
  color_dialog->setAttribute(Qt::WA_DeleteOnClose);

  connect(color_dialog, &QColorDialog::currentColorChanged,
    [=](const QColor& color)
  {
    QString selected_color = color.name(QColor::HexArgb);
    change_plot_colors(plot, selected_color);
  });

  connect(color_dialog, &QColorDialog::colorSelected,
    [=](const QColor& color)
  {
    QString selected_color = color.name(QColor::HexArgb);
    if (!dark_theme)
    {
      plot->default_changed = true;
      plot->default_color = selected_color;
    }
    else
    {
      plot->dark_changed = true;
      plot->dark_color = selected_color;
    }
  });

  connect(color_dialog, &QDialog::rejected, [=]
  {
    if (!dark_theme)
    {
      change_plot_colors(plot, plot->default_color);
    }
    else
    {
      change_plot_colors(plot, plot->dark_color);
    }
  });

  color_dialog->show();
}

void graph_widget::setup_ui()
{
  pause_run_button = new QPushButton();
  pause_run_button->setObjectName("pause_run_button");
  pause_run_button->setText(tr("&Pause"));
  pause_run_button->setMinimumSize(pause_run_button->sizeHint());
  connect(pause_run_button, &QPushButton::clicked,
    this, &graph_widget::pause_or_run);

  custom_plot = new QCustomPlot();
  custom_plot->axisRect()->setAutoMargins(QCP::msNone);
  custom_plot->setAntialiasedElements(QCP::aePlottables);
  custom_plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

  connect(custom_plot, SIGNAL(mousePress(QMouseEvent*)),
    this, SLOT(mouse_press(QMouseEvent*)));

  domain = new QSpinBox();
  domain->setValue(10);  // show a time span of 10 seconds by default
  domain->setRange(1, max_domain_ms / 1000);

  show_all_none = new QPushButton("Show &all/none");
  show_all_none->setObjectName("show_all_none");
  show_all_none->setStyleSheet("QPushButton{padding: 4px;}");

  connect(show_all_none, SIGNAL(clicked()),
    this, SLOT(show_all_none_clicked()));

  controls_layout = new QGridLayout();
  controls_layout->addWidget(show_all_none, 0, 0);
  controls_layout->addWidget(new QLabel("Position:"), 0, 1, Qt::AlignCenter);
  controls_layout->addWidget(new QLabel("Scale:"), 0, 2, Qt::AlignCenter);

  QHBoxLayout * bottom_control_layout = new QHBoxLayout();
  bottom_control_layout->addWidget(new QLabel(tr(" Time (s):")), 0, Qt::AlignRight);
  bottom_control_layout->addWidget(domain, 0);
  bottom_control_layout->addWidget(pause_run_button, 0, Qt::AlignRight);

  setup_plot(input, "input", "Input",
    "#00ffff", "#ff355e", 4095);

  setup_plot(target, "target", "Target",
    "#0000ff", "#ff6037", 4095, true);

  // The original Jrk software used #ffc0cb for feedback, but that is kind of
  // hard to see when we use it as a text color.
  setup_plot(feedback, "feedback", "Feedback",
    "#ff00aa", "#ffcc33", 4095);

  setup_plot(scaled_feedback, "scaled_feedback", "Scaled feedback",
    "#ff0000", "#ccff00", 4095, true);

  setup_plot(error, "error", "Error",
    "#9400d3", "#aaf0d1", 4095);

  setup_plot(integral, "integral", "Integral",
    "#ff8c00", "#ff6eff", 0x7fff);

  setup_plot(duty_cycle_target, "duty_cycle_target", "Duty cycle target",
    "#32cd32", "#fd5b78", 600);

  setup_plot(duty_cycle, "duty_cycle", "Duty cycle",
    "#006400", "#ff9933", 600);

  setup_plot(raw_current, "raw_current",
    "Raw current (mV)", "#660066", "#ffff66", 4095);

  setup_plot(current, "current", "Current (mA)",
    "#b8860b", "#66ff66", 100000);

  setup_plot(current_chopping, "current_chopping",
    "Current chopping", "#d500ff", "#50bfe6", 1);

  QFrame * division_frame = new QFrame();
  division_frame->setFrameShadow(QFrame::Plain);
  division_frame->setLineWidth(0);
  division_frame->setFrameShape(QFrame::HLine);

  int col_span = controls_layout->columnCount();
  controls_layout->addWidget(division_frame, row++, 0, 1, col_span);
  controls_layout->addLayout(bottom_control_layout, row++, 0, 1, col_span,
    Qt::AlignCenter);

  QSharedPointer<QCPAxisTickerText> y_axis_ticker(new QCPAxisTickerText);

  y_axis_ticker->addTick(0, "");

  for (int i = 10; i <= 50; i += 10)
  {
    y_axis_ticker->addTick(i, "");
    y_axis_ticker->addTick(-i, "");
  }

  y_axis_ticker->setSubTickCount(1);

  custom_plot->yAxis->setTicker(y_axis_ticker);
  custom_plot->yAxis->setTickLengthOut(1);

  QPen grid_pen(QColor(100, 100, 100, 140), 0, Qt::SolidLine);
  QPen sub_grid_pen(QColor(120, 120, 120, 110), 0, Qt::DashDotDotLine);

  custom_plot->xAxis->grid()->setPen(grid_pen);
  custom_plot->xAxis2->grid()->setPen(grid_pen);
  custom_plot->yAxis->grid()->setPen(grid_pen);
  custom_plot->yAxis->grid()->setZeroLinePen(grid_pen);
  custom_plot->yAxis->grid()->setSubGridPen(sub_grid_pen);
  custom_plot->yAxis->grid()->setSubGridVisible(true);
  custom_plot->yAxis->setSelectableParts(QCPAxis::spNone);

  custom_plot->yAxis->setRange(-50, 50);
  custom_plot->xAxis->setTickLabelPadding(10);
  custom_plot->xAxis->setLabelPadding(2);

  // Uncomment this line if you want to see "real" time scrolling by on the
  // top side of the graph.
  // custom_plot->xAxis2->setVisible(true);

  set_line_visible();

  custom_plot->axisRect()->setRangeDragAxes(0, 0);
  custom_plot->axisRect()->setRangeZoomAxes(0, 0);
  custom_plot->axisRect()->setRangeZoom(Qt::Vertical);
}

QMenuBar * graph_widget::setup_menu_bar()
{
  if (menu_bar) { return menu_bar; }

  menu_bar = new QMenuBar();

  QMenu * options_menu = menu_bar->addMenu(tr("&Options"));

  QAction * save_settings_action = new QAction(this);
  save_settings_action->setText("Save graph settings...");
  save_settings_action->setShortcut(Qt::CTRL + Qt::Key_S);

  QAction * load_settings_action = new QAction(this);
  load_settings_action->setText("Load graph settings...");
  load_settings_action->setShortcut(Qt::CTRL + Qt::Key_L);

  dark_theme_action = new QAction(this);
  dark_theme_action->setText(tr("&Use dark theme"));

  default_theme_action = new QAction(this);
  default_theme_action->setText(tr("&Use default theme"));
  default_theme_action->setVisible(false);

  QAction * reset_all_colors_action = new QAction(this);
  reset_all_colors_action->setText("Reset all &colors");

  QAction * reset_all_ranges_action = new QAction(this);
  reset_all_ranges_action->setText("&Reset all positions and scales");

  options_menu->addAction(save_settings_action);
  options_menu->addAction(load_settings_action);
  options_menu->addSeparator();
  options_menu->addAction(default_theme_action);
  options_menu->addAction(dark_theme_action);
  options_menu->addAction(reset_all_colors_action);
  options_menu->addAction(reset_all_ranges_action);

  connect(save_settings_action, &QAction::triggered, this,
    &graph_widget::save_settings);

  connect(load_settings_action, &QAction::triggered, this,
    &graph_widget::load_settings);

  connect(dark_theme_action, &QAction::triggered, this,
    &graph_widget::switch_to_dark);

  connect(default_theme_action, &QAction::triggered, this,
    &graph_widget::switch_to_default);

  connect(reset_all_colors_action, &QAction::triggered, this,
    &graph_widget::reset_all_colors);

  connect(reset_all_ranges_action, &QAction::triggered, this,
    &graph_widget::reset_all_ranges);

  return menu_bar;
}

void graph_widget::setup_plot(plot & plot,
  const QString & id_string, const QString & display_text,
  const QString & default_color, const QString & dark_color,
  int typical_max_value, bool default_visible)
{
  plot.index = all_plots.size();
  plot.id_string = id_string;
  plot.default_scale = typical_max_value / 5.0;

  plot.original_default_color = plot.default_color = default_color;
  plot.original_dark_color = plot.dark_color = dark_color;

  plot.scale = new dynamic_decimal_spin_box();
  plot.scale->setAccelerated(true);
  plot.scale->setRange(0.01, 1000000);
  plot.scale->setValue(plot.default_scale);

  plot.position = new QDoubleSpinBox();
  plot.position->setAccelerated(true);
  plot.position->setRange(-1000000, 1000000);
  plot.position->setValue(0);

  update_position_step_value(plot);

  plot.display = new QCheckBox();
  plot.display->setText(display_text);
  plot.display->setToolTip("Right-click to change plot options");
  set_checkbox_style(&plot, plot.default_color);
  plot.display->setCheckable(true);
  plot.display->setChecked(default_visible);
  plot.display->installEventFilter(this);

  plot.axis = custom_plot->axisRect()->addAxis(QCPAxis::atLeft);
  plot.axis->setRange(-typical_max_value, typical_max_value);
  plot.axis->setVisible(false);

  y_label_font.setPixelSize(35);

  plot.axis_label = new QCPItemText(custom_plot);
  plot.axis_label->setClipToAxisRect(false);
  plot.axis_label->setPositionAlignment(Qt::AlignRight | Qt::AlignVCenter);
  plot.axis_label->setFont(y_label_font);
  plot.axis_label->setColor(default_color);
  plot.axis_label->setText("\u27a4");
  plot.axis_label->setPadding(QMargins(10, 10, 10, 10));
  plot.axis_label->position->setTypeX(QCPItemPosition::ptAxisRectRatio);
  plot.axis_label->position->setTypeY(QCPItemPosition::ptPlotCoords);
  plot.axis_label->position->setAxisRect(custom_plot->axisRect());
  plot.axis_label->position->setAxes(0, plot.axis);
  plot.axis_label->position->setCoords(0, 0);

  QFont axis_label_text;
  axis_label_text.setPixelSize(11);

  plot.axis_position_label = new QCPItemText(custom_plot);
  plot.axis_position_label->setClipToAxisRect(false);
  plot.axis_position_label->setFont(axis_label_text);
  plot.axis_position_label->setColor(default_color);
  plot.axis_position_label->setTextAlignment(Qt::AlignCenter);
  plot.axis_position_label->setVisible(false);

  plot.axis_scale_label = new QCPItemText(custom_plot);
  plot.axis_scale_label->setClipToAxisRect(false);
  plot.axis_scale_label->setFont(axis_label_text);
  plot.axis_scale_label->setColor(default_color);
  plot.axis_scale_label->setTextAlignment(Qt::AlignCenter);
  plot.axis_scale_label->setVisible(false);

  x_label_font.setPixelSize(30);

  plot.overflow_arrows.append(axis_arrow(plot, 270));
  plot.overflow_arrows.append(axis_arrow(plot, 90));
  plot.overflow_arrows.append(axis_arrow(plot, 270));
  plot.overflow_arrows.append(axis_arrow(plot, 90));
  plot.overflow_arrows.append(axis_arrow(plot, 270));
  plot.overflow_arrows.append(axis_arrow(plot, 90));

  controls_layout->addWidget(plot.display, row, 0);
  controls_layout->addWidget(plot.position, row, 1);
  controls_layout->addWidget(plot.scale, row, 2);

  plot.graph = custom_plot->addGraph(custom_plot->xAxis2, plot.axis);
  plot.graph->setPen(QPen(QColor(plot.default_color), 1));

  // Briton says StepCenter mode helped with performance issues when dragging
  // the axis range.
  plot.graph->setLineStyle(QCPGraph::lsStepCenter);

  connect(plot.display, &QCheckBox::toggled, [=](bool checked)
  {
    set_line_visible();
  });

  connect(plot.axis, static_cast<void (QCPAxis::*)(const QCPRange &, const QCPRange &)>
    (&QCPAxis::rangeChanged), [=](const QCPRange & newRange, const QCPRange & oldRange)
  {
    double position_value = -(newRange.upper + newRange.lower) / 2.0;
    double scale_value = (-newRange.lower + newRange.upper) / 10.0;

    if (scale_value < plot.scale->minimum()
      || scale_value > plot.scale->maximum()
      || position_value < plot.position->minimum()
      || position_value > plot.position->maximum())
    {
      QSignalBlocker blocker(plot.axis);
      plot.axis->setRange(oldRange.lower, oldRange.upper);
    }

    {
      QSignalBlocker blocker(plot.position);
      plot.position->setValue(position_value);
    }

    {
      QSignalBlocker blocker(plot.scale);
      plot.scale->setValue(scale_value);
    }

    update_position_step_value(plot);
    update_plot_text_and_arrows(plot);
  });

  connect(plot.scale, static_cast<void (QDoubleSpinBox::*)(const QString &)>
    (&QDoubleSpinBox::valueChanged), [=](const QString & value)
  {
    set_range(plot);
  });

  connect(plot.position, static_cast<void (QDoubleSpinBox::*)(const QString &)>
    (&QDoubleSpinBox::valueChanged), [=](const QString & value)
  {
    set_range(plot);
  });

  all_plots.append(&plot);

  row++;
}

void graph_widget::update_x_axis()
{
  int domain_ms = domain->value() * 1000;
  custom_plot->xAxis->setRange(-domain_ms, 0);
  custom_plot->xAxis2->setRange(display_time, domain_ms, Qt::AlignRight);
}

// Removes old data that we will not need to display later so that the graph
// doesn't consume too much memory.
// Note: We are not doing anything to handle the uint32_t overflow
// of our time variable.
void graph_widget::remove_old_data()
{
  double oldest_displayable_time = (double)display_time - max_domain_ms - 1000;
  double oldest_later_displayable_time = (double)current_time - max_domain_ms - 1000;
  for (auto plot : all_plots)
  {
    plot->graph->data()->removeBefore(oldest_displayable_time);
    plot->graph->data()->remove(display_time, oldest_later_displayable_time);
  }
}

// Configures the specified plot so we can use the mouse to change its scale
// and position.  Display the scale and position labels.
void graph_widget::set_graph_interaction_axis(const plot & plot)
{
  plot.graph->setPen(QPen(plot.graph->pen().color(), 2));

  y_label_font.setPixelSize(40);
  x_label_font.setPixelSize(35);

  plot.axis_label->setFont(y_label_font);

  plot.axis_position_label->setVisible(true);
  plot.axis_scale_label->setVisible(true);

  // Move these items in front of other items.
  plot.axis_label->setLayer(plot.axis_label->layer());
  plot.axis_position_label->setLayer(plot.axis_position_label->layer());
  plot.axis_scale_label->setLayer(plot.axis_scale_label->layer());

  for (auto label : plot.overflow_arrows)
  {
    label->setFont(x_label_font);
  }

  custom_plot->axisRect()->setRangeDragAxes(0, plot.axis);
  custom_plot->axisRect()->setRangeZoomAxes(0, plot.axis);
}

// Clears selected axes from range drag and zoom.
void graph_widget::reset_graph_interaction_axes()
{
  custom_plot->axisRect()->setRangeDragAxes(0, 0);
  custom_plot->axisRect()->setRangeZoomAxes(0, 0);

  for (auto plot : all_plots)
  {
    plot->graph->setPen(QPen(plot->graph->pen().color(), 1));

    y_label_font.setPixelSize(35);
    x_label_font.setPixelSize(30);

    plot->axis_label->setFont(y_label_font);

    plot->axis_position_label->setVisible(false);
    plot->axis_scale_label->setVisible(false);

    for (auto label : plot->overflow_arrows)
    {
      label->setFont(x_label_font);
    }
  }
}

void graph_widget::update_plot_text_and_arrows(const plot & plot)
{
  plot.axis_position_label->setText("Position:\n" + plot.position->cleanText());
  plot.axis_scale_label->setText("Scale:\n" + plot.scale->cleanText());

  if (plot.axis->range().lower >= 0)
  {
    plot.axis_label->setRotation(90);
    plot.axis_label->setPositionAlignment(Qt::AlignTop | Qt::AlignHCenter);
    plot.axis_label->setPadding(QMargins(6, 0, 6, 0));
    plot.axis_label->position->setCoords(0, plot.axis->range().lower);

    plot.axis_scale_label->setPositionAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    plot.axis_position_label->setPositionAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    plot.axis_scale_label->position->setParentAnchor(plot.axis_label->left);
    plot.axis_position_label->position->setParentAnchor(plot.axis_scale_label->top);
  }
  else if (plot.axis->range().upper <= 0)
  {
    plot.axis_label->setRotation(270);
    plot.axis_label->setPositionAlignment(Qt::AlignBottom | Qt::AlignHCenter);
    plot.axis_label->setPadding(QMargins(6, 0, 6, 0));
    plot.axis_label->position->setCoords(0, plot.axis->range().upper);

    plot.axis_scale_label->setPositionAlignment(Qt::AlignHCenter | Qt::AlignTop);
    plot.axis_position_label->setPositionAlignment(Qt::AlignHCenter | Qt::AlignTop);
    plot.axis_scale_label->position->setParentAnchor(plot.axis_position_label->bottom);
    plot.axis_position_label->position->setParentAnchor(plot.axis_label->left);
  }
  else
  {
    plot.axis_label->setRotation(0);
    plot.axis_label->setPositionAlignment(Qt::AlignRight | Qt::AlignVCenter);
    plot.axis_label->setPadding(QMargins(10, -5, 10, -5));
    plot.axis_label->position->setCoords(0, 0);

    plot.axis_scale_label->setPositionAlignment(Qt::AlignHCenter | Qt::AlignTop);
    plot.axis_position_label->setPositionAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    plot.axis_scale_label->position->setParentAnchor(plot.axis_label->bottom);
    plot.axis_position_label->position->setParentAnchor(plot.axis_label->top);
  }

  update_plot_overflow_arrows(plot);
}

// Note: There is just barely enough space for the overflow arrows with our
// current number of plots (11).  If we add more plots, we might need to make
// the arrows smaller.
void graph_widget::update_plot_overflow_arrows(const plot & plot)
{
  bool plot_visible = plot.display->isChecked();

  bool out_top = false;
  bool out_bot = false;
  double arrow_offset = 0;
  int arrow_count = 0;

  if (plot_visible)
  {
    int domain_ms = domain->value() * 1000;
    auto begin = plot.graph->data()->findBegin((double)display_time - domain_ms);
    auto end = plot.graph->data()->findEnd((double)display_time);
    double upper = plot.axis->range().upper;
    double lower = plot.axis->range().lower;
    for (auto it = begin; it != end; ++it)
    {
      if (it->value > upper) { out_top = true; }
      if (it->value < lower) { out_bot = true; }
    }

    arrow_count = qBound(1, custom_plot->viewport().width() / 350, 3);

    arrow_offset = 0.05 + 0.9 * plot.index / (all_plots.size() - 1);
  }

  plot.overflow_arrows[0]->setVisible(out_top && arrow_count >= 1);
  plot.overflow_arrows[1]->setVisible(out_bot && arrow_count >= 1);
  plot.overflow_arrows[2]->setVisible(out_top && arrow_count >= 2);
  plot.overflow_arrows[3]->setVisible(out_bot && arrow_count >= 2);
  plot.overflow_arrows[4]->setVisible(out_top && arrow_count >= 3);
  plot.overflow_arrows[5]->setVisible(out_bot && arrow_count >= 3);

  double top = plot.axis->range().upper;
  double bot = plot.axis->range().lower;

  switch (arrow_count)
  {
  case 1:
    plot.overflow_arrows[0]->position->setCoords(arrow_offset, top);
    plot.overflow_arrows[1]->position->setCoords(arrow_offset, bot);
    break;
  case 2:
    plot.overflow_arrows[0]->position->setCoords(arrow_offset / 2, top);
    plot.overflow_arrows[1]->position->setCoords(arrow_offset / 2, bot);
    plot.overflow_arrows[2]->position->setCoords((arrow_offset + 1) / 2, top);
    plot.overflow_arrows[3]->position->setCoords((arrow_offset + 1) / 2, bot);
    break;
  case 3:
    plot.overflow_arrows[0]->position->setCoords(arrow_offset / 3, top);
    plot.overflow_arrows[1]->position->setCoords(arrow_offset / 3, bot);
    plot.overflow_arrows[2]->position->setCoords((arrow_offset + 1) / 3, top);
    plot.overflow_arrows[3]->position->setCoords((arrow_offset + 1) / 3, bot);
    plot.overflow_arrows[4]->position->setCoords((arrow_offset + 2) / 3, top);
    plot.overflow_arrows[5]->position->setCoords((arrow_offset + 2) / 3, bot);
    break;
  }
}

void graph_widget::update_position_step_value(const plot & plot)
{
  plot.position->setSingleStep(plot.scale->value() / 10);
}

void graph_widget::reset_plot_range(const plot & plot)
{
  plot.axis->setRange(-plot.default_scale * 5.0, plot.default_scale * 5.0);
}

void graph_widget::set_range(const plot & plot)
{
  plot.display->setCheckState(Qt::Checked);

  reset_graph_interaction_axes();

  // Warning: These formulas are duplicated in load_settings().
  double lower_range = -(plot.scale->value() * 5.0) - plot.position->value();
  double upper_range = (plot.scale->value() * 5.0) - plot.position->value();

  {
    QSignalBlocker blocker(plot.axis);
    plot.axis->setRange(lower_range, upper_range);
  }

  update_position_step_value(plot);
  update_plot_text_and_arrows(plot);
  set_graph_interaction_axis(plot);
  custom_plot->replot();
}

QCPItemText * graph_widget::axis_arrow(const plot & plot, double degrees)
{
  QCPItemText * label_instance = new QCPItemText(custom_plot);
  label_instance->setClipToAxisRect(false);
  label_instance->setPositionAlignment(Qt::AlignCenter);
  label_instance->setFont(x_label_font);
  label_instance->setColor(plot.default_color);
  label_instance->setText("\u27a4");
  label_instance->setRotation(degrees);
  label_instance->position->setTypeX(QCPItemPosition::ptAxisRectRatio);
  label_instance->position->setTypeY(QCPItemPosition::ptPlotCoords);
  label_instance->position->setAxisRect(custom_plot->axisRect());
  label_instance->position->setAxes(0, plot.axis);
  label_instance->setVisible(false);
  return label_instance;
}

void graph_widget::save_settings()
{
  QString filename = QFileDialog::getSaveFileName(custom_plot,
    "Save Graph Settings", "jrk_graph_settings.txt", "Text files (*.txt)");

  if (filename.isEmpty()) { return; }

  QFile file_out(filename);
  if (file_out.open(QFile::WriteOnly | QFile::Text))
  {
    QTextStream out(&file_out);
    for (auto plot : all_plots)
    {
      out
        << plot->id_string << ','
        << plot->display->isChecked() << ','
        << plot->position->cleanText() << ','
        << plot->scale->cleanText() << ','
        << plot->default_color << ','
        << plot->dark_color << '\n';
    }
  }
  else
  {
    // TODO: report file save errors (e.g. file_out.errorMessage())
    return;
  }
}

void graph_widget::load_settings()
{
  QStringList all_plots_settings;

  QString filename = QFileDialog::getOpenFileName(custom_plot,
    "Load Graph Settings", "", "Text files (*.txt)");

  if (filename.isEmpty()) { return; }

  QFile file_in(filename);
  if (file_in.open(QFile::ReadOnly | QFile::Text))
  {
    QTextStream stream_in(&file_in);
    while (!stream_in.atEnd())
    {
      all_plots_settings += stream_in.readLine();
    }
  }
  else
  {
    // TODO: report file read errors (e.g. file_in.errorMessage())
    return;
  }

  for (int i = 0; i < all_plots_settings.size(); i++)
  {
    QStringList settings = all_plots_settings[i].split(",");

    if (settings[3].toDouble() < 0.1)
    {
      settings[3] = "0.1";
    }

    // TODO: instead of using all_plots[i], locate the specified plot using its
    // name.  This also saves us from an array overflow bug.

    all_plots[i]->display->setChecked(settings[1].toInt());
    // Warning: These formulas are duplicated in set_range().
    double lower_range = -(settings[3].toDouble() * 5.0) - (settings[2].toDouble());
    double upper_range = (settings[3].toDouble() * 5.0) - (settings[2].toDouble());
    all_plots[i]->axis->setRange(lower_range, upper_range);

    all_plots[i]->default_color = settings[4];
    all_plots[i]->dark_color = settings[5];
  }

  if (dark_theme)
  {
    switch_to_dark();
  }
  else
  {
    switch_to_default();
  }
}

void graph_widget::switch_to_dark()
{
  for (auto plot : all_plots)
  {
    change_plot_colors(plot, plot->dark_color);
  }

  QLinearGradient axis_rect_gradient;
  axis_rect_gradient.setStart(0, 0);
  axis_rect_gradient.setFinalStop(0, 350);
  axis_rect_gradient.setColorAt(0, QColor(40, 40, 40));
  axis_rect_gradient.setColorAt(1, QColor(10, 10, 10));
  custom_plot->axisRect()->setBackground(axis_rect_gradient);
  custom_plot->setBackground(QColor(170, 170, 170));
  custom_plot->xAxis->grid()->pen().setColor(QColor(225, 225, 225));
  custom_plot->yAxis->grid()->pen().setColor(QColor(225, 225, 225));
  custom_plot->xAxis->setBasePen(QPen(QColor(Qt::white), 1, Qt::SolidLine));
  custom_plot->yAxis->setBasePen(QPen(QColor(Qt::white), 1, Qt::SolidLine));
  custom_plot->yAxis->grid()->zeroLinePen().setColor(QColor(225, 225, 225));

  dark_theme_action->setVisible(false);
  default_theme_action->setVisible(true);

  dark_theme = true;

  custom_plot->replot();
}

void graph_widget::switch_to_default()
{
  for (auto plot : all_plots)
  {
    change_plot_colors(plot, plot->default_color);
  }

  custom_plot->axisRect()->setBackground(QColor(Qt::white));
  custom_plot->setBackground(QColor(Qt::white));
  custom_plot->xAxis->grid()->pen().setColor(QColor(100, 100, 100));
  custom_plot->yAxis->grid()->pen().setColor(QColor(100, 100, 100));
  custom_plot->xAxis->setBasePen(QPen(QColor(Qt::black), 1, Qt::SolidLine));
  custom_plot->yAxis->setBasePen(QPen(QColor(Qt::black), 1, Qt::SolidLine));
  custom_plot->yAxis->grid()->zeroLinePen().setColor(QColor(100, 100, 100));

  dark_theme_action->setVisible(true);
  default_theme_action->setVisible(false);

  dark_theme = false;

  custom_plot->replot();
}

void graph_widget::change_ranges(int domain)
{
  update_x_axis();
  custom_plot->replot();
}

void graph_widget::pause_or_run()
{
  set_paused(!graph_paused);
}

void graph_widget::set_line_visible()
{
  reset_graph_interaction_axes();

  for (auto plot : all_plots)
  {
    plot->graph->setVisible(plot->display->isChecked());
    plot->axis_label->setVisible(plot->display->isChecked());
    update_plot_text_and_arrows(*plot);
  }

  custom_plot->replot();
}

void graph_widget::show_all_none_clicked()
{
  bool all_checked = std::all_of(all_plots.begin(), all_plots.end(),
    [](const plot * plot) { return plot->display->isChecked(); });

  for (auto plot : all_plots)
  {
    QSignalBlocker blocker(plot->display);
    plot->display->setChecked(!all_checked);
  }

  set_line_visible();
}

void graph_widget::reset_all_colors()
{
  QMessageBox mbox(QMessageBox::Question, "Reset all colors",
    QString::fromStdString("Reset all colors to default?"),
    QMessageBox::Ok | QMessageBox::Cancel, custom_plot);

  if (mbox.exec() != QMessageBox::Ok) { return; }

  for (auto plot : all_plots)
  {
    plot->default_color = plot->original_default_color;
    plot->default_changed = false;
    plot->dark_color = plot->original_dark_color;
    plot->dark_changed = false;

    if (dark_theme)
    {
      change_plot_colors(plot, plot->dark_color);
    }
    else
    {
      change_plot_colors(plot, plot->default_color);
    }
  }
}

// Resets all position and scale values to default.
void graph_widget::reset_all_ranges()
{
  QMessageBox mbox(QMessageBox::Question, "Reset all positions",
    "Reset all positions and scales?",
    QMessageBox::Ok | QMessageBox::Cancel, custom_plot);

  if (mbox.exec() != QMessageBox::Ok) { return; }

  for (auto plot : all_plots)
  {
    reset_plot_range(*plot);
  }
  reset_graph_interaction_axes();
  custom_plot->replot();
}

// Receives a click event from Qt and figures out which plot to select, if any.
//
// Note: We use "distance <= click_distance" below so that if two axis arrows
// are exactly on top of each other, the click will choose the one that
// has a later index (i.e. is lower on the list of plots).  That's a good
// default since the arrows for plots with the later indices are on top by
// default.  As soon as people start moving the axes around and changing the
// Z ordering, this falls apart.
void graph_widget::mouse_press(QMouseEvent * event)
{
  reset_graph_interaction_axes();

  plot * plot_clicked = NULL;

  // Mouse click position tolerances.
  const double arrow_tolerance = 8.0;
  const double plot_tolerance = 5.0;
  double click_distance = 999;

  for (auto plot : all_plots)
  {
    if (!plot->display->isChecked()) { continue; }

    double distance = plot->axis_label->selectTestPololu(event->localPos(), false);
    if (distance <= arrow_tolerance && distance <= click_distance)
    {
      plot_clicked = plot;
      click_distance = distance;
    }

    for (auto arrow : plot->overflow_arrows)
    {
      if (!arrow->visible()) { continue; }

      distance = arrow->selectTestPololu(event->localPos(), false);
      if (distance <= arrow_tolerance && distance <= click_distance)
      {
        plot_clicked = plot;
        click_distance = distance;
      }
    }

    if (!plot->graph->data()->isEmpty())
    {
      double distance = plot->graph->selectTest(event->localPos(), false);
      if (distance > 0 && distance <= plot_tolerance && distance <= click_distance)
      {
        plot_clicked = plot;
        click_distance = distance;
      }
    }
  }

  if (plot_clicked == NULL) { return; }

  if (event->button() == Qt::RightButton)
  {
    show_plot_menu(plot_clicked, true);
  }
  else
  {
    set_graph_interaction_axis(*plot_clicked);
  }
  custom_plot->replot();
}

void dynamic_decimal_spin_box::stepBy(int steps)
{
  int val = qRound(value() * 100);
  while (steps > 0)
  {
    val = step_up(val);
    steps--;
  }
  while (steps < 0)
  {
    val = step_down(val);
    steps++;
  }
  setValue(val / 100.0);
  selectAll();
}

// step_up behaves much the same way a person might behave when entering an
// increasing sequence of numbers without a specific value in mind.  It finds
// the decimal digit that is one digit after the first non-zero decimal digit
// (or the same digit if that is not possible), and then increases or decreases
// that digit while setting the later digits to zero.
//
// This means our step size is always between 1% and 10%, and after taking
// a step there are at most two non-zero decimal digits.
//
// Note: We assume the integers used in this function and step_down are pretty
// far from overflowing or underflowing (by a factor of 1000 at least).
// INT_MIN will result in a stack overflow, and the core calculations could
// underflow or overflow if that is not the case.
int dynamic_decimal_spin_box::step_up(int value)
{
  // If the value is negative, negate it and use step_down so that the core
  // algorithms don't have to deal with negative numbers.
  if (value < 0)
  {
    return -step_down(-value);
  }
  int change = 1;
  while (change * 100 <= value) { change *= 10; }
  return (value + change) / change * change;
}

// Does the opposite of step_up.  Some tweaks were needed to make
// sure that (for example) 561 through 569 map to 560 instead of 550, and
// 100 maps to 99 instead of 90.
int dynamic_decimal_spin_box::step_down(int value)
{
  if (value < 0)
  {
    return -step_up(-value);
  }
  int change = 1;
  while (change * 100 < value) { change *= 10; }
  return (value - 1) / change * change;
}

// Show 0 to 2 digits after the decimal point, making sure to show at least
// 3 significant digits if we can, so that the steps taken by stpeBy are
// always visible.
QString dynamic_decimal_spin_box::textFromValue(double value) const
{
  if (qFabs(value) >= 100 || value == 0)
  {
    return QString::number(value, 'f', 0);
  }
  else if (qFabs(value) >= 10)
  {
    return QString::number(value, 'f', 1);
  }
  else
  {
    return QString::number(value, 'f', 2);
  }
}

double dynamic_decimal_spin_box::valueFromText(const QString & text) const
{
  return text.toDouble();
}
