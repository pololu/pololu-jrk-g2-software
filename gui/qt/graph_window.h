#pragma once

#include "graph_widget.h"

#include <QWidget>

class QGridLayout;
class QHBoxLayout;
class QPushButton;
class QVBoxLayout;

class AltWindow : public QWidget
{
  Q_OBJECT
public:
  AltWindow(QWidget *parent = 0);
  ~AltWindow();




  void setup_ui();
  graph_widget *grabbedWidget;
  void closeEvent(QCloseEvent *);
  QWidget *centralwidget;
  QVBoxLayout *verticalLayout;
  QPushButton *backBtn;
  QHBoxLayout *horizontal_layout;
  QGridLayout *centralLayout;
  QGridLayout *mainLayout;
  QHBoxLayout *plotLayout;
  QHBoxLayout *bottomControlLayout;
  QVBoxLayout *plotRangeLayout;
  QVBoxLayout *plotVisibleLayout;
  QVBoxLayout *range_label_layout;
  void retranslate_ui();

signals:
  void pass_widget(graph_widget* widget);

public slots:
  void receive_widget(graph_widget *widget);
};
