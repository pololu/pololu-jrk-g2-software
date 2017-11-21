#pragma once

#include <QMainWindow>
#include "graphwindow.h"

class AltWindow : public QMainWindow
{
    Q_OBJECT
public:
    AltWindow(QWidget *parent = 0);
    ~AltWindow();




    void setupUi(QMainWindow *AltWindow);
    GraphWindow *grabbedWidget;
    void closeEvent(QCloseEvent *);
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QPushButton *backBtn;
    QHBoxLayout *horizontalLayout;
    QGridLayout *centralLayout;
    QGridLayout *mainLayout;
    QHBoxLayout *plotLayout;
    QHBoxLayout *bottomControlLayout;
    QVBoxLayout *plotRangeLayout;
    QVBoxLayout *plotVisibleLayout;
    void retranslateUi(QMainWindow *AltWindow);

signals:
    void passWidget(GraphWindow* widget);

public slots:
    void receiveWidget(GraphWindow *widget);
};