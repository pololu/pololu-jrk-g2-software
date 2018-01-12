#pragma once

#include "graph_widget.h"

#include <QWidget>

class QGridLayout;
class QHBoxLayout;
class QPushButton;
class QVBoxLayout;

class graph_window : public QWidget
{
  Q_OBJECT
public:
  graph_window(QWidget *parent = 0);


private:
  graph_widget *grabbed_widget;
  QGridLayout *central_layout;
  QHBoxLayout *plot_layout;
  QHBoxLayout *bottom_control_layout;
  QVBoxLayout *plot_visible_layout;

  void setup_ui();
  void closeEvent(QCloseEvent *);
  void retranslate_ui();

signals:
  void pass_widget(graph_widget* widget);

public slots:
  void receive_widget(graph_widget *widget);
};
