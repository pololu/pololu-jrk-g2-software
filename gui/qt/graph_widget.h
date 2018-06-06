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
#include <QLineEdit>
#include <QObject>
#include <QScreen>
#include <QColorDialog>
#include <QAction>
#include <QMenu>
#include <QMessageBox>

class dynamic_decimal_spinbox;

class graph_widget : public QWidget
{
  Q_OBJECT

public:
  graph_widget(QWidget * parent = 0);

  struct plot
  {
    dynamic_decimal_spinbox *scale;
    dynamic_decimal_spinbox * position;
    QCheckBox *display;
    QString default_color;
    QString dark_color;
    QString original_default_color;
    QString original_dark_color;
    QCPAxis *axis;
    QCPGraph *graph;
    int32_t plot_value = 0;
    QPushButton *reset_button;
    QCPItemText * axis_label;
    QCPItemText * axis_position_label;
    QCPItemText * axis_scale_label;
    QList<QCPItemText *> axis_top_and_bottom;
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
  bool dark_theme = false;

  void set_preview_mode(bool preview_mode);
  void set_paused(bool paused);
  void clear_graphs();
  void plot_data(uint32_t time);

  void change_plot_colors(plot * plot, const QString&);

protected:
  bool eventFilter(QObject * o, QEvent * e);

private:
  void set_plot_color(plot * x);
  void setup_ui();

  // Used to add new plot
  void setup_plot(plot& x, QString display_text, QString default_color,
    QString dark_color, double scale, bool default_visible = false);

  void remove_data_to_scroll(uint32_t time);
  void set_graph_interaction_axis(plot x);
  void reset_graph_interaction_axes();
  void set_axis_text(plot x);
  void set_range(plot x);

  QCPItemText * axis_arrow(plot x, double degrees);
  QPushButton *pause_run_button;
  QSpinBox *domain;
  QPushButton *show_all_none;
  QPushButton *reset_all_button;

  QFont y_label_font;
  QFont x_label_font;

  uint32_t key; // used to store local copy of time value

  int row = 1;
  bool in_preview = false; // used to store local copy of preview_mode
  int viewport_width = 0; // used to determine location of mouse press event.

private slots:
  void change_ranges();
  void on_pause_run_button_clicked();
  void set_line_visible();
  void show_all_none_clicked();
  void on_reset_all_button_clicked();
  void mouse_press(QMouseEvent*);
};

// This subclass of QDoubleSpinBox is used to add more control to both the
// value displayed in the QDoubleSpinBox and the steps caused by "arrow up",
// "arrow down", "page up", and "page down" keys.
class dynamic_decimal_spinbox : public QDoubleSpinBox {   Q_OBJECT

public:
  explicit dynamic_decimal_spinbox(QWidget * parent = 0) :
    QDoubleSpinBox(parent) {}

  QSize minimumSizeHint() const;

protected:
  virtual void stepBy(int step_value);
  virtual StepEnabled stepEnabled();
  QString textFromValue ( double value ) const;
  double valueFromText ( const QString & text ) const;

private:
  double calculate_decimal_step(int steps);
};
