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
	QWidget *centralWidget;
	QVBoxLayout *verticalLayout;
	QHBoxLayout *horizontalLayout;
	GraphWindow *previewWindow;
	QPushButton *separateBtn;
	AltWindow *altw;
	QWidget *redWidget;
	QWidget *previewPlot;
	

	QAction *sepAct;
	bool widgetAtHome;
	

	void setupUi(QMainWindow *MainWindow);
	void retranslateUi(QMainWindow *MainWindow);

signals:
	void passWidget(GraphWindow *widget);

private slots:
	void on_separateBtn_clicked();
	void receiveWidget(GraphWindow *widget);
	void on_launchGraph_clicked(QMouseEvent*);

	

protected:
	void contextMenuEvent(QContextMenuEvent *event);

	
};