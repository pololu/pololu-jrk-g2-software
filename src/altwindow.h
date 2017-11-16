#pragma once

#include <QMainWindow>
#include "graphwindow.h"

class AltWindow : public QMainWindow
{
    Q_OBJECT
public:
    AltWindow(QWidget *parent = 0);
    ~AltWindow();

    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QPushButton *backBtn;
    QHBoxLayout *horizontalLayout;

private:
    void setupUi(QMainWindow *);
    GraphWindow *grabbedWidget;
    void closeEvent(QCloseEvent *);
signals:
    void passWidget(GraphWindow* widget);

public slots:
    void receiveWidget(GraphWindow *widget);

};