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
  pause_run_button = new QPushButton(this);
  pause_run_button->setObjectName("pause_run_button");
  pause_run_button->setCheckable(true);
  pause_run_button->setChecked(false);
  pause_run_button->setText(tr("&Pause"));

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

  input.range = new QDoubleSpinBox();
  input.display = new QCheckBox("Input");
  input.display->
    setStyleSheet(QStringLiteral("border: 5px solid #00ffff; background-color: white"));
  input.color = "#00ffff";
  input.range_label = new QLabel("0-");
  input.graph_data_selection_bar = new QHBoxLayout();
  input.range_value = 4095;

  target.range = new QDoubleSpinBox();
  target.display = new QCheckBox("Target");
  target.display->
    setStyleSheet(QStringLiteral("border: 5px solid #0000ff; background-color: white"));
  target.color = "#0000ff";
  target.range_label = new QLabel("0-");
  target.graph_data_selection_bar = new QHBoxLayout();
  target.range_value = 4095;

  feedback.range = new QDoubleSpinBox();
  feedback.display = new QCheckBox("Feedback");
  feedback.display->
    setStyleSheet(QStringLiteral("border: 5px solid #ffc0cb; background-color: white"));
  feedback.color = "#ffc0cb";
  feedback.range_label = new QLabel("0-");
  feedback.graph_data_selection_bar = new QHBoxLayout();
  feedback.range_value = 4095;

  scaled_feedback.range = new QDoubleSpinBox();
  scaled_feedback.display = new QCheckBox("Scaled Feedback");
  scaled_feedback.display->
    setStyleSheet(QStringLiteral("border: 5px solid #ff0000; background-color: white"));
  scaled_feedback.color = "#ff0000";
  scaled_feedback.range_label = new QLabel("0-");
  scaled_feedback.graph_data_selection_bar = new QHBoxLayout();
  scaled_feedback.range_value = 4095;

  error.range = new QDoubleSpinBox();
  error.display = new QCheckBox("Error");
  error.display->
    setStyleSheet(QStringLiteral("border: 5px solid #9400d3; background-color: white"));
  error.color = "#9400d3";
  error.range_label = new QLabel("\u00B1");
  error.graph_data_selection_bar = new QHBoxLayout();
  error.range_value = 4095;

  integral.range = new QDoubleSpinBox();
  integral.display = new QCheckBox("Integral");
  integral.display->
    setStyleSheet(QStringLiteral("border: 5px solid #ff8c00; background-color: white"));
  integral.color = "#ff8c00";
  integral.range_label = new QLabel("\u00B1");
  integral.graph_data_selection_bar = new QHBoxLayout();
  integral.range_value = 1000;

  duty_cycle_target.range = new QDoubleSpinBox();
  duty_cycle_target.display = new QCheckBox("Duty cycle target");
  duty_cycle_target.display->
    setStyleSheet(QStringLiteral("border: 5px solid #32cd32; background-color: white"));
  duty_cycle_target.color = "#32cd32";
  duty_cycle_target.range_label = new QLabel("\u00B1");
  duty_cycle_target.graph_data_selection_bar = new QHBoxLayout();
  duty_cycle_target.range_value = 600;

  duty_cycle.range = new QDoubleSpinBox();
  duty_cycle.display = new QCheckBox("Duty cycle");
  duty_cycle.display->
    setStyleSheet(QStringLiteral("border: 5px solid #006400; background-color: white"));
  duty_cycle.color = "#006400";
  duty_cycle.range_label = new QLabel("\u00B1");
  duty_cycle.graph_data_selection_bar = new QHBoxLayout();
  duty_cycle.range_value = 600;

  raw_current.range = new QDoubleSpinBox();
  raw_current.display = new QCheckBox("Raw current (mV)");
  raw_current.display->
    setStyleSheet(QStringLiteral("border: 5px solid #b8860b; background-color: white"));
  raw_current.color = "#b8860b";
  raw_current.range_label = new QLabel("0\u2013");
  raw_current.graph_data_selection_bar = new QHBoxLayout();
  raw_current.range_value = 4095;

  current.range = new QDoubleSpinBox();
  current.display = new QCheckBox("Current (mA)");
  current.display->
    setStyleSheet(QStringLiteral("border: 5px solid #0000ff; background-color: white;"));
  current.color = "#0000ff";
  current.range_label = new QLabel("\u00B1");
  current.graph_data_selection_bar = new QHBoxLayout();
  current.range_value = 100000;

  current_chopping_log.range = new QDoubleSpinBox();
  current_chopping_log.display = new QCheckBox("Current chopping log");
  current_chopping_log.display->
    setStyleSheet(QStringLiteral("border: 5px solid #ff00ff; background-color: white;"));
  current_chopping_log.color = "#ff00ff";
  current_chopping_log.range_label = new QLabel("0\u2013");
   // TODO: use en dashes on the other plots too
  current_chopping_log.graph_data_selection_bar = new QHBoxLayout();
  current_chopping_log.range_value = 1;

  all_plots.append(&input);
  all_plots.append(&target);
  all_plots.append(&feedback);
  all_plots.append(&scaled_feedback);
  all_plots.append(&error);
  all_plots.append(&integral);
  all_plots.append(&duty_cycle_target);
  all_plots.append(&duty_cycle);
  all_plots.append(&raw_current);
  all_plots.append(&current);
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
// connects the valueChanged signal of the range to change the ranges of the plots.
{
  for (auto plot : all_plots)
  {
    plot->range->setDecimals(0);
    plot->range->setSingleStep(1.0);

    plot->axis = custom_plot->axisRect(0)->addAxis(QCPAxis::atRight);

    plot->axis->setVisible(false);

    plot->range_label->setStyleSheet(QStringLiteral("font: 14px"));

    plot->axis->setRange(-plot->range_value, plot->range_value);

    plot->range->setRange(0, plot->range_value);
    plot->range->setValue(plot->range_value);
    // Set the size of the labels and buttons for the errors tab in
    // a way that can change from OS to OS.
    {
      QLabel tmp_label2;
      tmp_label2.setText("xxxxxxxxxxxxx");
      QLabel tmp_label3;
      tmp_label3.setText("xxxxxxxxxxxxxxxxxxxxxxxxxx");
      plot->range->setFixedWidth(tmp_label2.sizeHint().width());
      plot->display->setFixedWidth(tmp_label3.sizeHint().width());
    }

    plot->display->setCheckable(true);
    plot->display->setChecked(true);

    plot->graph_data_selection_bar->setMargin(0);;
    plot->graph_data_selection_bar->addWidget(plot->display);
    plot->graph_data_selection_bar->addWidget(plot->range_label);
    plot->graph_data_selection_bar->addWidget(plot->range);

    plot->graph = new QCPGraph(custom_plot->xAxis2,plot->axis);

    plot->graph->setPen(QPen(plot->color));

    connect(plot->range, SIGNAL(valueChanged(double)), this, SLOT(change_ranges()));

    connect(plot->display, SIGNAL(clicked()), this, SLOT(set_line_visible()));
  }
}

void graph_widget::change_ranges()
{
  for (auto plot : all_plots)
  {
    plot->axis->setRangeUpper((plot->range->value()) * ((max_y->value())/100));
    plot->axis->setRangeLower((plot->range->value()) * ((min_y->value())/100));
  }
  custom_plot->yAxis->setRange(min_y->value(), max_y->value());
  custom_plot->replot();
}

void graph_widget::on_pause_run_button_clicked()
// used the QMetaObject class to parse the function name and send the signal to the function.
{
  pause_run_button->setText(pause_run_button->isChecked() ? "R&un" : "&Pause");
  custom_plot->replot();
}

void graph_widget::set_line_visible()
// sets the visibility of the plot both when the plot is running and when it is paused.
{
  for (auto plot : all_plots)
  {
    plot->graph->setVisible(plot->display->isChecked());
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
    plot->graph->addData(key, plot->plot_value);
    plot->graph->removeDataBefore(key - domain->value());
  }

  remove_data_to_scroll();
}

