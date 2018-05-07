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
#include <QApplication>
#include <QRadioButton>
#include <QButtonGroup>

class graph_widget : public QWidget
{
  Q_OBJECT

public:
  graph_widget(QWidget * parent = 0);

  struct plot
  {
    QDoubleSpinBox *scale;
    QDoubleSpinBox * position;
    QCheckBox *display;
    QString default_color;
    QString dark_color;
    QCPAxis *axis;
    QCPGraph *graph;
    int32_t plot_value = 0;
    double range_value = 0;
    bool default_visible = false;
    QPushButton *reset_button;
    QString plot_name;
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

  QCustomPlot * custom_plot;
  QGridLayout * plot_visible_layout;

  bool graph_paused = false;

  void set_preview_mode(bool preview_mode);
  void set_paused(bool paused);
  void clear_graphs();
  void plot_data(uint32_t time);

private:
  void setup_ui();

  // Used to add new plot
  void setup_plot(plot& x, QString display_text, QString default_color,
    QString dark_color, bool signed_range, double scale,
    bool default_visible = false);

  void remove_data_to_scroll(uint32_t time);
  void set_graph_interaction_axis(QCPAxis*, QCPGraph*);
  void reset_graph_interaction_axes();

  QWidget *central_widget;
  QPushButton *pause_run_button;
  QSpinBox *domain;
  QPushButton *show_all_none;
  QPushButton *reset_all_button;

  uint32_t key; // used to store local copy of time value

  int row = 1;

private slots:
  void change_ranges();
  void on_pause_run_button_clicked();
  void set_line_visible();
  void show_all_none_clicked();
  void on_reset_all_button_clicked();
  void graph_clicked(QMouseEvent*);
};
