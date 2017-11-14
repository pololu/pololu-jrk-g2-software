#include <QMainWindow>
#include <QList>
#include "qcustomplot.h"

class GraphWindow;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget * parent = 0);
	~MainWindow();
	void main_window_ui();

	QTimer DataTimer;
	GraphWindow *previewWindow;

public slots:
	void on_launchGraph_clicked(QMouseEvent * event);
};

class GraphWindow : public QWidget
{
	Q_OBJECT
public:
	GraphWindow(QWidget * parent = 0);

	struct Plot
    {
    	QDoubleSpinBox *plotRange;
    	QCheckBox *plotDisplay;
    	QCPAxis *plotAxis;
    	QCPGraph *plotGraph;
    };

    Plot yellowLine;
    Plot blueLine;
    Plot greenLine;
    Plot redLine;

    double key = 0;
	QTimer dataTimer;
	QWidget *centralWidget;
    QCustomPlot *customPlot;
    QPushButton *pauseRunButton;
    QPushButton *runButton;
    QDoubleSpinBox *minY;
    QDoubleSpinBox *maxY;
    QSpinBox *domain;
    QLabel *label1;
    QLabel *label2;
    QLabel *label3;

    QGridLayout *controlLayout;

    QList<Plot> allPlots;

public slots:
	void on_pauseRunButton_clicked();
	void remove_data_to_scroll();
	void realtimeDataSlot();
	void setupUi();
	void set_line_visible();
	void setupPlots();
	void changeRanges();

protected:
	
	
};