#pragma once

#include "qcustomplot.h"

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QPushButton>
#include <QString>
#include <QWidget>

class dynamic_decimal_spinbox;

class graph_widget : public QWidget
{
  Q_OBJECT

public:
  graph_widget(QWidget * parent = 0);

  // The maximum time span that can be displayed, in milliseconds.
  const int max_domain_ms = 90000;

  struct plot
  {
    int index = 0;
    QString id_string;
    dynamic_decimal_spinbox * scale;
    dynamic_decimal_spinbox * position;
    QCheckBox * display;
    QString default_color;
    QString dark_color;
    QString original_default_color;
    QString original_dark_color;
    bool default_changed = false;
    bool dark_changed = false;
    QCPAxis * axis;
    QCPGraph * graph;
    int32_t plot_value = 0;
    QPushButton * reset_button;
    QCPItemText * axis_label;
    QCPItemText * axis_position_label;
    QCPItemText * axis_scale_label;
    QList<QCPItemText *> overflow_arrows;
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

  bool preview_mode = true;

  QMenuBar * setup_menu_bar();

  void set_preview_mode(bool preview_mode);
  void set_paused(bool paused);
  void clear_graphs();
  void plot_data(uint32_t time);

  void set_checkbox_style(plot *, const QString &);
  void change_plot_colors(plot *, const QString &);

protected:
  bool eventFilter(QObject * o, QEvent * e);

private:
  void show_color_change_menu(plot *, bool with_title);
  void set_plot_color(plot *);
  void setup_ui();

  QMenuBar * menu_bar;
  QMenu * options_menu;
  QAction * save_settings_action;
  QAction * load_settings_action;
  QAction * dark_theme_action;
  QAction * default_theme_action;

  // Used to add new plot
  void setup_plot(plot &,
    const QString & id_string, const QString & display_text,
    const QString & default_color, const QString & dark_color,
    double scale, bool default_visible = false);

  QCPItemText * axis_arrow(const plot &, double degrees);
  QPushButton * pause_run_button;
  QSpinBox * domain;
  QPushButton * show_all_none;
  QPushButton * reset_all_button;

  void update_x_axis();
  void remove_old_data();
  void set_graph_interaction_axis(const plot &);
  void reset_graph_interaction_axes();
  void update_plot_text_and_arrows(const plot &);
  void update_plot_overflow_arrows(const plot &);
  void set_range(const plot &);
  void set_plot_grid_colors(int value);

  QFont y_label_font;
  QFont x_label_font;

  // time value corresponding to the right edge of the graph
  uint32_t display_time;

  // time value corresponding to the latest data from the device.
  // Should equal display_time if we are not paused.
  uint32_t current_time;

  int row = 1;
  bool graph_paused = false;
  bool dark_theme = false;

public slots:
  void save_settings();
  void load_settings();

private slots:
  void switch_to_dark();
  void switch_to_default();
  void change_ranges(int value);
  void on_pause_run_button_clicked();
  void set_line_visible();
  void show_all_none_clicked();
  void on_reset_all_button_clicked();
  void mouse_press(QMouseEvent *);
};

// This subclass of QDoubleSpinBox is used to add more control to both the
// value displayed in the QDoubleSpinBox and the steps caused by "arrow up",
// "arrow down", "page up", and "page down" keys.
class dynamic_decimal_spinbox : public QDoubleSpinBox
{
  Q_OBJECT

public:
  explicit dynamic_decimal_spinbox(QWidget * parent = 0) :
    QDoubleSpinBox(parent) {}

protected:
  virtual void stepBy(int step_value);
  QString textFromValue(double value) const;
  double valueFromText(const QString & text) const;

private:
  double calculate_decimal_step(int steps);
};
