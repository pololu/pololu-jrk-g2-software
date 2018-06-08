#pragma once

#include "graph_widget.h"

#include <QWidget>

class QGridLayout;

class graph_window : public QWidget
{
  Q_OBJECT
public:
  graph_window(QWidget * parent = 0);

private:
  graph_widget * grabbed_widget;
  QGridLayout * central_layout;

  void setup_ui();
  void closeEvent(QCloseEvent *);

signals:
  void pass_widget();

public slots:
  void receive_widget(graph_widget * widget);
  void raise_window();
};
