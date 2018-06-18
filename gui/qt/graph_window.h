#pragma once

#include "graph_widget.h"

#include <QWidget>

class QGridLayout;

class graph_window : public QWidget
{
  Q_OBJECT
public:
  graph_window();

private:
  graph_widget * widget;
  QGridLayout * central_layout;

  void setup_ui();
  void closeEvent(QCloseEvent *);

signals:
  void pass_widget();

public slots:
  void receive_widget(graph_widget * widget);
  void raise_window();
};
