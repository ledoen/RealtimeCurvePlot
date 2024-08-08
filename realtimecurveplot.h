#ifndef REALTIMECURVEPLOT_H
#define REALTIMECURVEPLOT_H

#include <QWidget>
#include "curveplotfifo.h"
#include "qcustomplot.h"

class RealtimeCurvePlot : public QWidget
{
    Q_OBJECT
public:
    explicit RealtimeCurvePlot(CurvePlotFifo* fifo, int channelCount, QWidget *parent = nullptr);

signals:
private:
    CurvePlotFifo* fifo;
    int channelCount;
    QCustomPlot* customPlot;
    QTimer replotTimer;
private:
    void initCurvePlot();
private slots:
    void refreshCurve();
    void onLegendClick(QCPLegend *legend, QCPAbstractLegendItem *item, QMouseEvent *event);
};

#endif // REALTIMECURVEPLOT_H
