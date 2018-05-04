#pragma once

#include "graph_widget.h"

#include <QWidget>
#include <QApplication>
#include <QtPrintSupport/QPrinter>
#include <QPainter>
#include <QFileDialog>
#include <QStyle>
#include <QMenuBar>
#include <QPixmap>

class QGridLayout;

class graph_window : public QWidget
{
  Q_OBJECT
public:
  graph_window(QWidget *parent = 0);

private:
  graph_widget *grabbed_widget;
  QGridLayout *central_layout;

  QMenuBar * menu_bar;
  QMenu * options_menu;
  QAction * save_settings_action;
  QAction * load_settings_action;
  QAction * dark_theme_action;
  QAction * default_theme_action;

  bool dark_theme = false;

  void setup_ui();
  void closeEvent(QCloseEvent *);

signals:
  void pass_widget();

public slots:
  void receive_widget(graph_widget *widget);
  void raise_window();
  void save_settings();
  void load_settings();
  void switch_to_dark();
  void switch_to_default();
};
