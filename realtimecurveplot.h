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
    QComboBox *displayLenthComboBox;
    int displayLengthInSecond = 5;
private:
    void initLayout();
    void initCurvePlot();
    void initDisplayLengthComboBox();
private slots:
    void refreshCurve();
    void onLegendClick(QCPLegend *legend, QCPAbstractLegendItem *item, QMouseEvent *event);
    void onDisplayLengthChanged(int index);
};

#endif // REALTIMECURVEPLOT_H
