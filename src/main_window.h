#include <QMainWindow>
#include "qcustomplot.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow(QWidget * parent = 0);

private slots:
	void setupPlayground(QCustomPlot * customPlot);
	void on_pauseRunButton_clicked();
	void Receivedata();
	void realtimeDataSlot();
	void setupUi();
	void retranslateUi();

private:
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
};
