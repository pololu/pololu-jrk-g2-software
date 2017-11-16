#pragma once

#include <QMainWindow>
#include <QList>
#include <QAction>
#include <QMenu>
#include <QContextMenuEvent>
#include "qcustomplot.h"
#include "graphwindow.h"
#include "altwindow.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget * parent = 0);
	~MainWindow();
	QTimer DataTimer;
	QGridLayout *layout;
	QWidget *central;
	QVBoxLayout *verticalLayout;
	QHBoxLayout *horizontalLayout;
	GraphWindow *previewWindow;
	QPushButton *separateBtn;
	AltWindow *altw;

	QAction *sepAct;
	bool widgetAtHome;
	

	void main_window_ui(QMainWindow *MainWindow);

signals:
	void passWidget(GraphWindow *widget);

private slots:
	void on_launchGraph_clicked();
	void receiveWidget(GraphWindow *widget);

	

protected:
	void contextMenuEvent(QContextMenuEvent *event);

	
};