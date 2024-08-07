#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "curveplotfifo.h"
#include "realtimecurveplot.h"
#include <QThread>

#define CURVE_CHANNEL_COUNT 10

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    CurvePlotFifo* fifo;
    QThread generatorThread;
    RealtimeCurvePlot* plot;
};
#endif // MAINWINDOW_H
