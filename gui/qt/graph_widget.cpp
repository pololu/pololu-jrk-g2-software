#include "graph_widget.h"
#include "main_window.h"
#include "qcustomplot.h"

#include <QApplication>
#include <QCheckBox>
#include <QCloseEvent>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QSpinBox>
#include <QVector>
#include <QWidget>

#include <array>
#include <ctime>


graph_widget::graph_widget(QWidget * parent)
{
  setup_ui();
  setup_plots();

  connect(max_y, SIGNAL(valueChanged(double)), this, SLOT(change_ranges()));
  connect(min_y, SIGNAL(valueChanged(double)), this, SLOT(change_ranges()));
  connect(domain, SIGNAL(valueChanged(int)), this, SLOT(remove_data_to_scroll()));
}

void graph_widget::setup_ui()
// sets the gui objects in the graph window
{
  this->setObjectName(tr("graph_widget"));

  central_widget = new QWidget(this);
  central_widget->setObjectName(tr("central_widget"));

  pauseRunButton = new QPushButton();
  pauseRunButton->setObjectName("pauseRunButton");
  pauseRunButton->setCheckable(true);
  pauseRunButton->setChecked(false);
  pauseRunButton->setText(tr("&Pause"));

  connect(pauseRunButton, SIGNAL(clicked()), this, SLOT(on_pauseRunButton_clicked()));

  QFont font;
  font.setPointSize(8);

  label1 = new QLabel();
  label1->setFont(font);
  label1->setText(tr("Range (%)"));

  custom_plot = new QCustomPlot();

  label2 = new QLabel();
  label2->setFont(font);
  label2->setText(tr("Time (s)"));

  label3 = new QLabel();
  label3->setFont(font);
  label3->setAlignment(Qt::AlignCenter);
  label3->setText(tr("-"));

  min_y = new QDoubleSpinBox();
  min_y->setRange(-100,0);
  min_y->setDecimals(0);
  min_y->setSingleStep(1.0);
  min_y->setValue(-100);

  max_y = new QDoubleSpinBox();
  max_y->setRange(0,100);
  max_y->setDecimals(0);
  max_y->setSingleStep(1.0);
  max_y->setValue(100);

  domain = new QSpinBox();
  domain->setValue(10); // initialized the graph to show 10 seconds of data

  input.plot_range = new QDoubleSpinBox();
  input.plot_display = new QCheckBox("Input");
  input.plot_display->
    setStyleSheet(QStringLiteral("border:5px solid #00ffff; text-align:center; font:14px; background-color:white"));
  input.color = "#00ffff";
  input.range_label = new QLabel("0-");
  input.graph_data_selection_bar = new QHBoxLayout();
  input.range_value = 4095;

  target.plot_range = new QDoubleSpinBox();
  target.plot_display = new QCheckBox("Target");
  target.plot_display->
    setStyleSheet(QStringLiteral("border:5px solid #0000ff; text-align:center; font:14px; background-color:white"));
  target.color = "#0000ff";
  target.range_label = new QLabel("0-");
  target.graph_data_selection_bar = new QHBoxLayout();
  target.range_value = 4095;

  feedback.plot_range = new QDoubleSpinBox();
  feedback.plot_display = new QCheckBox("Feedback");
  feedback.plot_display->
    setStyleSheet(QStringLiteral("border:5px solid #ffc0cb; text-align:center; font:14px; background-color:white"));
  feedback.color = "#ffc0cb";
  feedback.range_label = new QLabel("0-");
  feedback.graph_data_selection_bar = new QHBoxLayout();
  feedback.range_value = 4095;

  scaled_feedback.plot_range = new QDoubleSpinBox();
  scaled_feedback.plot_display = new QCheckBox("Scaled Feedback");
  scaled_feedback.plot_display->
    setStyleSheet(QStringLiteral("border:5px solid #ff0000; text-align:center; font:14px; background-color:white"));
  scaled_feedback.color = "#ff0000";
  scaled_feedback.range_label = new QLabel("0-");
  scaled_feedback.graph_data_selection_bar = new QHBoxLayout();
  scaled_feedback.range_value = 4095;

  error.plot_range = new QDoubleSpinBox();
  error.plot_display = new QCheckBox("Error");
  error.plot_display->
    setStyleSheet(QStringLiteral("border:5px solid #9400d3; text-align:center; font:14px; background-color:white"));
  error.color = "#9400d3";
  error.range_label = new QLabel("\u00B1");
  error.graph_data_selection_bar = new QHBoxLayout();
  error.range_value = 4095;

  integral.plot_range = new QDoubleSpinBox();
  integral.plot_display = new QCheckBox("Integral");
  integral.plot_display->
    setStyleSheet(QStringLiteral("border:5px solid #ff8c00; text-align:center; font:14px; background-color:white"));
  integral.color = "#ff8c00";
  integral.range_label = new QLabel("\u00B1");
  integral.graph_data_selection_bar = new QHBoxLayout();
  integral.range_value = 1000;

  duty_cycle_target.plot_range = new QDoubleSpinBox();
  duty_cycle_target.plot_display = new QCheckBox("Duty cycle target");
  duty_cycle_target.plot_display->
    setStyleSheet(QStringLiteral("border:5px solid #32cd32; text-align:center; font:14px; background-color:white"));
  duty_cycle_target.color = "#32cd32";
  duty_cycle_target.range_label = new QLabel("\u00B1");
  duty_cycle_target.graph_data_selection_bar = new QHBoxLayout();
  duty_cycle_target.range_value = 600;

  duty_cycle.plot_range = new QDoubleSpinBox();
  duty_cycle.plot_display = new QCheckBox("Duty cycle");
  duty_cycle.plot_display->
    setStyleSheet(QStringLiteral("border:5px solid #006400; text-align:center; font:14px; background-color:white"));
  duty_cycle.color = "#006400";
  duty_cycle.range_label = new QLabel("\u00B1");
  duty_cycle.graph_data_selection_bar = new QHBoxLayout();
  duty_cycle.range_value = 600;

  raw_current.plot_range = new QDoubleSpinBox();
  raw_current.plot_display = new QCheckBox("Raw current (mV)");
  raw_current.plot_display->
    setStyleSheet(QStringLiteral("border:5px solid #b8860b; text-align:center; font:14px; background-color:white"));
  raw_current.color = "#b8860b";
  raw_current.range_label = new QLabel("0\u2013");
  raw_current.graph_data_selection_bar = new QHBoxLayout();
  raw_current.range_value = 4095;

  scaled_current.plot_range = new QDoubleSpinBox();
  scaled_current.plot_display = new QCheckBox("Scaled current (mV)");
  scaled_current.plot_display->
    setStyleSheet(QStringLiteral("border:5px solid #0000ff; text-align:center; font:14px; background-color:white"));
  scaled_current.color = "#0000ff";
  scaled_current.range_label = new QLabel("\u00B1");
  scaled_current.graph_data_selection_bar = new QHBoxLayout();
  scaled_current.range_value = 2457000;

  current_chopping_log.plot_range = new QDoubleSpinBox();
  current_chopping_log.plot_display = new QCheckBox("Current chopping log");
  // TODO: why is the name cut off?  "log" is not shown
  current_chopping_log.plot_display->
    setStyleSheet(QStringLiteral("border:5px solid #ff00ff; text-align:center; font:14px; background-color:white"));
  current_chopping_log.color = "#ff00ff";
  current_chopping_log.range_label = new QLabel("0\u2013");
   // TODO: use en dashes on the other plots too
  current_chopping_log.graph_data_selection_bar = new QHBoxLayout();
  current_chopping_log.range_value = 65535;

  all_plots.append(&input);
  all_plots.append(&target);
  all_plots.append(&feedback);
  all_plots.append(&scaled_feedback);
  all_plots.append(&error);
  all_plots.append(&integral);
  all_plots.append(&duty_cycle_target);
  all_plots.append(&duty_cycle);
  all_plots.append(&raw_current);
  all_plots.append(&scaled_current);
  all_plots.append(&current_chopping_log);

  custom_plot->xAxis->QCPAxis::setRangeReversed(true);
  custom_plot->yAxis->setRange(-100,100);
  custom_plot->xAxis->setTickLabelType(QCPAxis::ltNumber);
  custom_plot->xAxis->setAutoTickStep(false);
  custom_plot->xAxis->setTickStep(1000);

  // this is used to see the x-axis to see accurate time.
  // custom_plot->xAxis2->setVisible(true);
  // custom_plot->xAxis2->setTickLabelType(QCPAxis::ltNumber);
  // custom_plot->xAxis2->setAutoTickStep(true);
  // custom_plot->xAxis2->setTickStep(2);

  QMetaObject::connectSlotsByName(this);
}

void graph_widget::setup_plots()
// sets the range QDoubleSpinBox, display QCheckBox, and the axis of the plot.
// connects the valueChanged signal of the plot_range to change the ranges of the plots.
{
  for (auto plot : all_plots)
  {
    plot->plot_range->setDecimals(0);
    plot->plot_range->setSingleStep(1.0);

    plot->plot_axis = custom_plot->axisRect(0)->addAxis(QCPAxis::atRight);

    plot->plot_axis->setVisible(false);

    plot->range_label->setStyleSheet(QStringLiteral("font: 14px"));

    plot->plot_axis->setRange(-plot->range_value, plot->range_value);

    plot->plot_range->setRange(0, plot->range_value);
    plot->plot_range->setValue(plot->range_value);
    // Set the size of the labels and buttons for the errors tab in
    // a way that can change from OS to OS.
    {
      QLabel tmp_label2;
      tmp_label2.setText("xxxxxxxxxxxxx");
      QLabel tmp_label3;
      tmp_label3.setText("xxxxxxxxxxxxxxxxxxxxxxx");
      plot->plot_range->setFixedWidth(tmp_label2.sizeHint().width());
      plot->plot_display->setFixedWidth(tmp_label3.sizeHint().width());
    }

    plot->plot_display->setCheckable(true);
    plot->plot_display->setChecked(true);

    plot->graph_data_selection_bar->setMargin(0);;
    plot->graph_data_selection_bar->addWidget(plot->plot_display, -1, Qt::AlignLeft);
    plot->graph_data_selection_bar->addWidget(plot->range_label, 2, Qt::AlignRight);
    plot->graph_data_selection_bar->addWidget(plot->plot_range, -1, Qt::AlignRight);

    plot->plot_graph = new QCPGraph(custom_plot->xAxis2,plot->plot_axis);

    plot->plot_graph->setPen(QPen(plot->color));

    connect(plot->plot_range, SIGNAL(valueChanged(double)), this, SLOT(change_ranges()));

    connect(plot->plot_display, SIGNAL(clicked()), this, SLOT(set_line_visible()));
  }
}

void graph_widget::change_ranges()
{
  for (auto plot : all_plots)
  {
    plot->plot_axis->setRangeUpper((plot->plot_range->value()) * ((max_y->value())/100));
    plot->plot_axis->setRangeLower((plot->plot_range->value()) * ((min_y->value())/100));
  }
  custom_plot->yAxis->setRange(min_y->value(), max_y->value());
  custom_plot->replot();
}

void graph_widget::on_pauseRunButton_clicked()
// used the QMetaObject class to parse the function name and send the signal to the function.
{
  pauseRunButton->setText(pauseRunButton->isChecked() ? "R&un" : "&Pause");
  custom_plot->replot();
}

void graph_widget::set_line_visible()
// sets the visibility of the plot both when the plot is running and when it is paused.
{
  for (auto plot : all_plots)
  {
    plot->plot_display->isChecked() ? plot->plot_graph->setVisible(true) : plot->plot_graph->setVisible(false);
    custom_plot->replot();
  }
}

void graph_widget::remove_data_to_scroll()
// modifies the x-axis based on the domain value and removes data outside of visible range
{
  custom_plot->xAxis->setRange(0, domain->value()*1000);

  // make key axis range scroll with the data at a rate of the time value obtained from QSpinBox
  custom_plot->xAxis2->setRange(key, domain->value(), Qt::AlignRight);

  // these range values are divided by 100 just for the sample data in order to draw sine curve
  custom_plot->replot();
}

// plots data on graph (TODO: better function name)
void graph_widget::realtime_data_slot()
{
  key += (refreshTimer/1000);

  for (auto plot : all_plots)
  {
    plot->plot_graph->addData(key, plot->plot_value);
    plot->plot_graph->removeDataBefore(key - domain->value());
  }

  remove_data_to_scroll();
}

