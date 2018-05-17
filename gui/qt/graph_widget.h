#pragma once

#include "qcustomplot.h"

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QWidget>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QApplication>
#include <QFontDatabase>
#include <QString>

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
    // QSharedPointer<QCPAxisTickerText> plot_axis_ticker;
    QCPItemText * axis_label;
    QCPItemText * axis_top_label;
    QCPItemText * axis_top_label2;
    QCPItemText * axis_top_label3;
    QCPItemText * axis_bottom_label;
    QCPItemText * axis_bottom_label2;
    QCPItemText * axis_bottom_label3;
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
  void set_graph_interaction_axis(plot x);
  void reset_graph_interaction_axes();
  void set_axis_text(plot plot);

  QWidget *central_widget;
  QPushButton *pause_run_button;
  QSpinBox *domain;
  QPushButton *show_all_none;
  QPushButton *reset_all_button;

  QFont font;
  QFont font2;

  uint32_t key; // used to store local copy of time value

  int row = 1;
  bool in_preview = false;

private slots:
  void change_ranges();
  void on_pause_run_button_clicked();
  void set_line_visible();
  void show_all_none_clicked();
  void on_reset_all_button_clicked();
  void graph_clicked(QMouseEvent*);
};
