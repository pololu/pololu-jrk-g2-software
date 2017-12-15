#pragma once

#include <QMainWindow>
#include <QList>
#include "qcustomplot.h"

class graph_widget : public QWidget
{
	Q_OBJECT

public:
	graph_widget(QWidget * parent = 0);
	~graph_widget();

	struct plot
    {
    	QDoubleSpinBox *plot_range;
    	QCheckBox *plot_display;
    	QCPAxis *plot_axis;
    	QCPGraph *plot_graph;
    };

  plot input;
  plot target;
  plot feedback;
  plot scaled_feedback;
  plot error;
  plot integral;
  plot duty_cycle_target;
  plot duty_cycle;
  plot current;

  double key = 0;
	QTimer data_timer;
	QWidget *central_widget;
  QCustomPlot *custom_plot;
  QPushButton *pauseRunButton;
  QDoubleSpinBox *min_y;
  QDoubleSpinBox *max_y;
  QSpinBox *domain;
  QLabel *label1;
  QLabel *label2;
  QLabel *label3;
  double refreshTimer = 25;

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
    "#b8860b"
  };

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
