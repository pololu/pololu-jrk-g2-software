#pragma once

#include "qcustomplot.h"
#include <QHBoxLayout>

class QCheckBox;
class QDoubleSpinBox;
class QLabel;
class QPushButton;
class QSpinBox;

class graph_widget : public QWidget
{
	Q_OBJECT

public:
	graph_widget(QWidget * parent = 0);

	struct plot
    {
    	QDoubleSpinBox *plot_range;
    	QCheckBox *plot_display;
    	QCPAxis *plot_axis;
    	QCPGraph *plot_graph;
      QLabel *range_label;
      QHBoxLayout *graph_data_selection_bar;
      int32_t plot_value = 0;
      bool double_ended_range = false;
      double range_value = 0;
    };

  plot input;
  plot target;
  plot feedback;
  plot scaled_feedback;
  plot error;
  plot integral;
  plot duty_cycle_target;
  plot duty_cycle;
  plot raw_current;
  plot scaled_current;
  plot current_chopping_log;

  double key = 0;
  QWidget *central_widget;
  QCustomPlot *custom_plot;
  QPushButton *pauseRunButton;
  QDoubleSpinBox *min_y;
  QDoubleSpinBox *max_y;
  QSpinBox *domain;
  QLabel *label1;
  QLabel *label2;
  QLabel *label3;
  double refreshTimer;



  QList<plot> all_plots;
  QList<QString> plot_colors = {
    "#00ffff",
    "#0000ff",
    "#ffc0cb",
    "#ff0000",
    "#9400d3",
    "#ff8c00",
    "#32cd32",
    "#006400",
    "#b8860b",
    "#0000ff",
    "#ff00ff",
  };
  // TODO: why do we have plot colors here and also in graph_widget.cpp?

signals:


public slots:
	void on_pauseRunButton_clicked();
	void remove_data_to_scroll();
	void realtime_data_slot();
	void setup_ui();
	void set_line_visible();
	void setup_plots();
	void change_ranges();




};
