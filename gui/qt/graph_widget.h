#pragma once

#include "qcustomplot.h"

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QSpinBox>
#include <QWidget>
#include <QHBoxLayout>
#include <QGridLayout>

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
  plot current_chopping;

  QCustomPlot *custom_plot;

  QHBoxLayout *bottom_control_layout;
  QGridLayout *plot_visible_layout;

  bool graph_paused = false;

  void set_preview_mode(bool preview_mode);
  void clear_graphs();
  void plot_data(uint32_t time);

private:
  void setup_ui();

  // Used to add new plot
  void setup_plot(plot& x, QString display_text, QString color,
    bool signed_range, double range, bool default_visible = false);

  void remove_data_to_scroll(uint32_t time);

  QWidget *central_widget;
  QPushButton *pause_run_button;
  QDoubleSpinBox *min_y;
  QDoubleSpinBox *max_y;
  QSpinBox *domain;
  QLabel *label1;
  QLabel *label2;
  QLabel *label3;

  uint32_t key; // used to store local copy of time value

  int row = 0;

private slots:
  void change_ranges();
  void on_pause_run_button_clicked();
  void set_line_visible();
};
