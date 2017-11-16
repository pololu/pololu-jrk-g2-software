#include <QMainWindow>
#include <QList>
#include "qcustomplot.h"


// class MainWindow : public QMainWindow
// {
// 	Q_OBJECT

// public:
// 	explicit MainWindow(QWidget * parent = 0);
// 	~MainWindow();

// 	QTimer DataTimer;
// 	GraphWindow *previewWindow;
// 	QGridLayout *layout;
// 	QWidget *central;

// public slots:
// 	void on_launchGraph_clicked(QMouseEvent * event);
// 	void main_window_ui();
// };

class GraphWindow : public QWidget
{
	Q_OBJECT

public:
	GraphWindow(QWidget * parent = 0);
	~GraphWindow();
	
private:
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
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;

    QGridLayout *controlLayout;

    QList<Plot> allPlots;

    QWidget *grabbedWidget;

    void closeEvent(QCloseEvent *);

signals:
	void passWidget(QWidget *widget);

public slots:
	void on_pauseRunButton_clicked();
	void remove_data_to_scroll();
	void realtimeDataSlot();
	void setupUi(QMainWindow *GraphWindow);
	void set_line_visible();
	void setupPlots();
	void changeRanges();
	void receiveWidget(QWidget *widget);


	
};