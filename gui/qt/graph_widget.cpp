#include "graph_widget.h"

#include <QString>
#include <QMessageBox>

graph_widget::graph_widget(QWidget * parent)
{
  setup_ui();

  connect(domain, SIGNAL(valueChanged(int)),
    this, SLOT(change_ranges()));
}

// Changes options for the custom_plot when in preview mode.
void graph_widget::set_preview_mode(bool preview_mode)
{
  if (preview_mode)
  {
    custom_plot->setCursor(Qt::PointingHandCursor);
    custom_plot->setToolTip("Click to open graph window");
    custom_plot->axisRect()->setMargins(QMargins(5, 5, 5, 5));
    custom_plot->xAxis->basePen().setWidthF(0);
    custom_plot->yAxis->basePen().setWidthF(0);

    custom_plot->axisRect()->setRangeDragAxes(0, 0);
    custom_plot->axisRect()->setRangeZoomAxes(0, 0);

    QRadioButton * temp_button = new QRadioButton();
    plot_interaction_radios->addButton(temp_button, -1);
    temp_button->setChecked(true);
  }
  else
  {
    custom_plot->setCursor(Qt::ArrowCursor);
    custom_plot->setToolTip("");
    custom_plot->axisRect()->setMargins(QMargins(20, 10, 20, 40));
    custom_plot->xAxis->basePen().setWidthF(1);
    custom_plot->yAxis->basePen().setWidthF(1);
  }

  for(auto plot : all_plots)
  {
    if (plot->display->isChecked())
    {
      plot->axis->setVisible(!preview_mode);
    }
  }

  custom_plot->xAxis->setTicks(!preview_mode);
  custom_plot->yAxis->setTicks(!preview_mode);

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
  for (auto plot : all_plots)
  {
    plot->graph->addData(time, plot->plot_value);
  }

  if (graph_paused)
    return;

  remove_data_to_scroll(time);
}

void graph_widget::setup_ui()
{
  pause_run_button = new QPushButton(this);
  pause_run_button->setObjectName("pause_run_button");
  pause_run_button->setText(tr("&Pause"));
  pause_run_button->setMinimumSize(pause_run_button->sizeHint());

  custom_plot = new QCustomPlot();
  custom_plot->axisRect()->setAutoMargins(QCP::msNone);
  custom_plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

  connect(custom_plot, SIGNAL(mousePress(QMouseEvent*)),
    this, SLOT(graph_clicked(QMouseEvent*)));

  domain = new QSpinBox();
  domain->setValue(10); // initialized the graph to show 10 seconds of data
  domain->setRange(1, 90);

  show_all_none = new QPushButton("Show all/none");
  show_all_none->setObjectName("show_all_none");

  plot_interaction_radios = new QButtonGroup();
  plot_interaction_radios->setExclusive(true);

  reset_all_button = new QPushButton(tr("Reset all"), this);
  reset_all_button->setStyleSheet("QPushButton{padding: 3px 2px 3px 2px;}");
  reset_all_button->setObjectName("reset_all_button");
  reset_all_button->setToolTip("Reset all plots\nposition and scale");

  connect(show_all_none, SIGNAL(clicked()),
    this, SLOT(show_all_none_clicked()));

  plot_visible_layout = new QGridLayout();
  plot_visible_layout->addWidget(show_all_none, 0, 1);
  plot_visible_layout->addWidget(new QLabel("Position:"), 0, 2, Qt::AlignCenter);
  plot_visible_layout->addWidget(new QLabel("Scale:"), 0, 3, Qt::AlignCenter);
  plot_visible_layout->addWidget(reset_all_button, 0, 4, Qt::AlignLeft);


  QHBoxLayout * bottom_control_layout = new QHBoxLayout();
  bottom_control_layout->addWidget(new QLabel(tr(" Time (s):")), 0, Qt::AlignRight);
  bottom_control_layout->addWidget(domain, 0);
  bottom_control_layout->addWidget(pause_run_button, 0, Qt::AlignRight);

  setup_plot(input, "Input", "#00ffff", "#ff355e", false, 4095);

  setup_plot(target, "Target", "#0000ff", "#ff6037", false, 4095, true);

  setup_plot(feedback, "Feedback", "#ff8296", "#ffcc33", false, 4095);

  setup_plot(scaled_feedback, "Scaled feedback", "#ff0000", "#ccff00", false, 4095, true);

  setup_plot(error, "Error", "#9400d3", "#aaf0d1", true, 4095);

  setup_plot(integral, "Integral", "#ff8c00", "#ff6eff", true, 0x7fff);

  setup_plot(duty_cycle_target, "Duty cycle target", "#32cd32", "#fd5b78", true, 600);

  setup_plot(duty_cycle, "Duty cycle", "#006400", "#ff9933", true, 600);

  setup_plot(raw_current, "Raw current (mV)", "#660066", "#ffff66", false, 4095);

  setup_plot(current, "Current (mA)", "#b8860b", "#66ff66", false, 100000);

  setup_plot(current_chopping, "Current chopping", "#ff00ff", "#50bfe6", false, 1);

  QFrame * division_frame = new QFrame();
  division_frame->setFrameShadow(QFrame::Plain);
  division_frame->setLineWidth(0);
  division_frame->setFrameShape(QFrame::HLine);

  plot_visible_layout->addWidget(division_frame, row, 0, 1, 5);
  plot_visible_layout->addLayout(bottom_control_layout, ++row, 0, 1, 5, Qt::AlignCenter);

  QSharedPointer<QCPAxisTickerFixed> x_axis_ticker(new QCPAxisTickerFixed);
  x_axis_ticker->setTickStepStrategy(QCPAxisTicker::tssReadability);
  x_axis_ticker->setScaleStrategy(QCPAxisTickerFixed::ssMultiples);
  custom_plot->xAxis->setTicker(x_axis_ticker);
  custom_plot->xAxis->setTickLengthOut(3);

  QSharedPointer<QCPAxisTickerText> y_axis_ticker(new QCPAxisTickerText);

  y_axis_ticker->addTick(0, "");

  for (int i = 10; i <= 50; (i += 10))
  {
    y_axis_ticker->addTick(i, "");
    y_axis_ticker->addTick(-i, "");
  }

  y_axis_ticker->setSubTickCount(1);

  custom_plot->yAxis->setTicker(y_axis_ticker);
  custom_plot->yAxis->setTickLengthOut(3);

  custom_plot->xAxis->grid()->setPen(QPen(QColor(100, 100, 100, 140), 0, Qt::SolidLine));
  custom_plot->xAxis2->grid()->setPen(QPen(QColor(100, 100, 100, 140), 0, Qt::SolidLine));

  custom_plot->yAxis->grid()->setPen(QPen(QColor(100, 100, 100, 140), 0, Qt::SolidLine));
  custom_plot->yAxis->grid()->setSubGridPen(QPen(QColor(120, 120, 120, 100), 0, Qt::DotLine));
  custom_plot->yAxis->grid()->setZeroLinePen(QPen(QColor(100, 100, 100, 140), 0, Qt::SolidLine));
  custom_plot->yAxis->grid()->setSubGridVisible(true);
  custom_plot->yAxis->setSelectableParts(QCPAxis::spNone);

  custom_plot->yAxis->setRange(-50,50);
  custom_plot->xAxis->setTickLabelPadding(10);
  custom_plot->xAxis->setLabel("Time (ms)");
  custom_plot->xAxis->setLabelPadding(2);

  // this is used to see the x-axis to see accurate time.
  // custom_plot->xAxis2->setVisible(true);

  set_line_visible();

  custom_plot->axisRect()->setRangeDragAxes(0, 0);
  custom_plot->axisRect()->setRangeZoomAxes(0, 0);
  custom_plot->axisRect()->setRangeZoom(Qt::Vertical);



  QMetaObject::connectSlotsByName(this);
}

void graph_widget::setup_plot(plot& plot, QString display_text, QString default_color,
  QString dark_color, bool signed_range, double range, bool default_visible)
{
  plot.default_color = default_color;
  plot.dark_color = dark_color;

  plot.range_value = range;

  plot.range = new QDoubleSpinBox();
  plot.range->setDecimals(1);
  plot.range->setSingleStep(1);
  plot.range->setAccelerated(true);
  plot.range->setRange(0, plot.range_value);
  plot.range->setValue(plot.range_value/5.0);

  plot.center_value = new QDoubleSpinBox();
  plot.center_value->setDecimals(0);
  plot.center_value->setSingleStep(1);
  plot.center_value->setAccelerated(true);
  plot.center_value->setRange(-plot.range_value, plot.range_value);

  plot.center_value->setValue(0);

  plot.display = new QCheckBox();
  plot.display->setText(display_text);
  plot.display->setStyleSheet("border: 2px solid "+ plot.default_color + ";"
    "padding: 2px;"
    "background-color: white;");
  plot.display->setCheckable(true);
  plot.display->setChecked(default_visible);

  plot.allow_interaction = new QRadioButton();
  plot.allow_interaction->setToolTip("Click to drag " + display_text + " plot postion");
  plot_interaction_radios->addButton(plot.allow_interaction, ++axis_index);

  plot.reset_button = new QPushButton(tr("\u27f2"));
  plot.reset_button->setStyleSheet("QPushButton{margin: 0px; padding: 2px;}");
  plot.reset_button->setToolTip("Reset " + display_text + " plot\nposition and scale");

  connect(plot.reset_button, &QPushButton::clicked, [=]
  {
    plot.range->setValue(plot.range_value/5.0);
    plot.center_value->setValue(0);

    custom_plot->replot();
  });

  QFont button_font;
  button_font.setPointSize(12);
  button_font.setBold(true);
  plot.reset_button->setFont(button_font);

  plot.default_visible = default_visible;

  plot.axis = custom_plot->axisRect()->addAxis(QCPAxis::atLeft);

  QSharedPointer<QCPAxisTickerText> plot_axis_ticker(new QCPAxisTickerText);
  plot_axis_ticker->setTickCount(1);
  plot_axis_ticker->setSubTickCount(0);
  plot_axis_ticker->addTick(0, "\u2B9E");
  plot.axis->setTicker(plot_axis_ticker);

  QFont font;
  font.setPointSize(14);

  plot.axis->setTickLabelFont(font);
  plot.axis->setTickLabelColor(default_color);
  plot.axis->setTickLabelPadding(0);
  plot.axis->setRange(-plot.range_value, plot.range_value);
  plot.axis->setBasePen(QPen(Qt::NoPen));
  plot.axis->setTickPen(QPen(Qt::NoPen));
  plot.axis->setSubTickPen(QPen(Qt::NoPen));
  plot.axis->grid()->setVisible(false);
  plot.axis->setSelectedBasePen(QPen(plot.default_color));
  plot.axis->setSelectedTickLabelColor(plot.default_color);

  connect(plot.axis, SIGNAL(mousePress(QMouseEvent*)),
    this, SLOT(graph_clicked(QMouseEvent*)));

  // plot_visible_layout->addWidget(plot.allow_interaction, row, 0);
  plot_visible_layout->addWidget(plot.display, row, 1);
  plot_visible_layout->addWidget(plot.center_value, row, 2);
  plot_visible_layout->addWidget(plot.range, row, 3);
  plot_visible_layout->addWidget(plot.reset_button, row, 4, Qt::AlignCenter);

  plot.graph = custom_plot->addGraph(custom_plot->xAxis2, plot.axis);
  plot.graph->setPen(QPen(plot.default_color));

  connect(plot.allow_interaction, &QRadioButton::clicked, [=]
  {
    plot.display->setChecked(true);
    set_line_visible();

    custom_plot->replot();
  });

  connect(plot.display, &QCheckBox::toggled, [=](bool checked)
  {
    if (checked)
    {
      QSignalBlocker blocker(plot.allow_interaction);
      plot.allow_interaction->click();
    }
    else if (!checked && plot.allow_interaction->isChecked())
    {
      //Removes the currently plot_interaction axis.
      custom_plot->axisRect()->setRangeDragAxes(0, 0);
      custom_plot->axisRect()->setRangeZoomAxes(0, 0);

      // Used to reset the plot_interaction_radios to un-checked.
      QRadioButton * temp_button = new QRadioButton();
      plot_interaction_radios->addButton(temp_button, -1);
      temp_button->setChecked(true);
    }

    set_line_visible();

    custom_plot->replot();
  });

  connect(plot_interaction_radios, static_cast<void (QButtonGroup::*)(int)>
    (&QButtonGroup::buttonClicked), [=](int id)
  {
    QList<QCPAxis *> drag_axes;
    drag_axes.append(custom_plot->axisRect()->axis(QCPAxis::atLeft, (id)));
    custom_plot->axisRect()->setRangeDragAxes(drag_axes);
    custom_plot->axisRect()->setRangeZoomAxes(drag_axes);
  });

  connect(plot.axis, static_cast<void (QCPAxis::*)(const QCPRange&)>
    (&QCPAxis::rangeChanged), [=](const QCPRange & newRange)
  {
    double position_value = -(newRange.lower + newRange.upper);

    plot.center_value->setRange(-newRange.upper, -newRange.lower);

    {
      QSignalBlocker blocker(plot.center_value);
      plot.center_value->setValue(position_value/2);
    }

    {
      QSignalBlocker blocker(plot.range);
      plot.range->setValue((-newRange.lower + newRange.upper)/10);
    }

    custom_plot->replot();
  });

  connect(plot.range, static_cast<void (QDoubleSpinBox::*)(double)>
    (&QDoubleSpinBox::valueChanged), [=](double value)
  {
    {
      QSignalBlocker blocker(plot.axis);
      plot.axis->setRangeLower(-(value * 5.0) - (plot.center_value->value()));
      plot.axis->setRangeUpper((value * 5.0) - (plot.center_value->value()));
    }

    plot.allow_interaction->click();

    custom_plot->replot();
  });

  connect(plot.center_value, static_cast<void (QDoubleSpinBox::*)(double)>
    (&QDoubleSpinBox::valueChanged), [=](double value)
  {
    {
      QSignalBlocker blocker(plot.axis);
      plot.axis->setRangeLower(-(plot.range->value() * 5.0) - (value));
      plot.axis->setRangeUpper((plot.range->value() * 5.0) - (value));
    }

    plot.allow_interaction->click();

    custom_plot->replot();
  });

  connect(plot.display, SIGNAL(clicked()), this, SLOT(set_line_visible()));

  all_plots.append(&plot);

  row++;
}

// modifies the x-axis based on the domain value
// and removes data outside of visible range
void graph_widget::remove_data_to_scroll(uint32_t time)
{
  key = time; // stores a local copy of time value

  custom_plot->xAxis->setRange(-domain->value() * 1000, 0);

  custom_plot->xAxis2->setRange(time, domain->value() * 1000, Qt::AlignRight);

  for (auto plot : all_plots)
    plot->graph->data()->removeBefore(domain->value() * 1000);

  custom_plot->replot();
}

void graph_widget::change_ranges()
{
  custom_plot->xAxis->setRange(-domain->value() * 1000, 0);

  custom_plot->xAxis2->setRange(key, domain->value() * 1000, Qt::AlignRight);

  custom_plot->replot();
}

void graph_widget::on_pause_run_button_clicked()
{
  set_paused(!graph_paused);
}

void graph_widget::set_line_visible()
{
  for (auto plot : all_plots)
  {
    if (plot->display->isChecked())
    {
      plot->graph->setSelectable(QCP::stWhole);
      plot->axis->setSelectableParts(QCPAxis::spTickLabels);
    }
    else
    {
      plot->graph->setSelectable(QCP::stNone);
      plot->axis->setSelectableParts(QCPAxis::spNone);
    }
    plot->graph->setVisible(plot->display->isChecked());
    plot->axis->setVisible(plot->display->isChecked());
  }

  custom_plot->replot();
}

void graph_widget::show_all_none_clicked()
{
  bool all_checked = std::all_of(all_plots.begin(), all_plots.end(),
    [](const plot * plot) {return plot->display->isChecked();});

  for (auto plot : all_plots)
  {
    {
      QSignalBlocker blocker(plot->display);
      if (all_checked)
        plot->display->setChecked(false);
      else
        plot->display->setChecked(true);
    }
  }

  set_line_visible();
}

void graph_widget::on_reset_all_button_clicked()
{
  QMessageBox mbox(QMessageBox::Question, "",
    QString::fromStdString("Reset all positions and scales?"),
    QMessageBox::Ok | QMessageBox::Cancel);
  mbox.exec();

  if (QMessageBox::Ok)
  {
    for (auto plot : all_plots)
    {
      plot->reset_button->click();
    }

    custom_plot->replot();
  }
  else
    return;
}

void graph_widget::graph_clicked(QMouseEvent * event)
{
  QList<QCPAxis *> drag_axes;
  QCPAxis * temp_axis = Q_NULLPTR;
  QCPGraph * temp_graph;

  double temp_value = 5;

  for(auto plot : custom_plot->axisRect()->graphs())
  {
    plot->setPen(QPen(plot->pen().color(), 1));

    double select_test_value = plot->selectTest(event->pos(), true);
    if (select_test_value != -1 &&
      select_test_value <= temp_value)
    {
      temp_axis = plot->valueAxis();
      temp_value = select_test_value;
      temp_graph = plot;
    }
  }

  if (temp_axis != Q_NULLPTR)
  {
    temp_graph->setPen(QPen(temp_graph->pen().color(), 2));
    custom_plot->replot();
    drag_axes.append(temp_axis);

    custom_plot->axisRect()->setRangeDragAxes(drag_axes);
    custom_plot->axisRect()->setRangeZoomAxes(drag_axes);
  }
}
