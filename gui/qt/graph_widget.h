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
    QDoubleSpinBox *range;
    QCheckBox *display;
    QString color;
    QCPAxis *axis;
    QCPGraph *graph;
    QLabel *range_label;
    QHBoxLayout *graph_data_selection_bar;
    int32_t plot_value = 0;
    double range_value = 0;
    bool default_visible = false;
  };

  QList<plot *> all_plots;
  plot input;
  plot target;
  plot feedback;
  plot scaled_feedback;
  plot error;
  plot integral;
  plot duty_cycle_target;
  plot duty_cycle;
  plot raw_current;
  plot current;
  plot current_chopping_log;

  double key = 0;
  QWidget *central_widget;
  QCustomPlot *custom_plot;
  QPushButton *pause_run_button;
  QDoubleSpinBox *min_y;
  QDoubleSpinBox *max_y;
  QSpinBox *domain;
  QLabel *label1;
  QLabel *label2;
  QLabel *label3;
  double refreshTimer;

private:
  void setup_ui();

  // Used to add new plot
  // display_text = label coupled with display checkbox
  // range_label = true ("0-")
  // range_label = false ("\u00B1")
  void setup_plot(plot& x, QString display_text, QString color,
  bool signed_range, double range, bool default_visible = false);

public slots:
  void on_pause_run_button_clicked();
	void remove_data_to_scroll();
	void realtime_data_slot();
	void set_line_visible();
	void change_ranges();
};
