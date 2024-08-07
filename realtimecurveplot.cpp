#include "realtimecurveplot.h"
#define SHOW_FPS_INFO 0

RealtimeCurvePlot::RealtimeCurvePlot(CurvePlotFifo* fifo, int channelCount, QWidget *parent)
    : QWidget{parent}
    , fifo(fifo)
    , channelCount(channelCount)
{
    initCurvePlot();

    connect(&replotTimer, SIGNAL(timeout()), this, SLOT(refreshCurve()));
    replotTimer.start(0);
}

/// 初始化plot，配置曲线数，坐标轴等
void RealtimeCurvePlot::initCurvePlot()
{
    customPlot = new QCustomPlot();
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(customPlot);

    // 根据传入的曲线个数，创建曲线
    for (int var = 0; var < channelCount; ++var)
    {
        customPlot->addGraph();
        QColor color;
        color.setHsl(var * 75 % 360, 255, 96);
        customPlot->graph(var)->setPen(color);
    }

    // 关闭拖动时抗锯齿，提高拖动时性能，避免拖动窗口时曲线变形
    customPlot->setNoAntialiasingOnDrag(true);

    // 设置坐标轴
    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    customPlot->xAxis->setTicker(timeTicker);
    customPlot->axisRect()->setupFullAxesBox();
    customPlot->yAxis->setRange(-1.2, 1.2);

    connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));
}

/// 添加数据，刷新图形
void RealtimeCurvePlot::refreshCurve()
{
    double key = QTime::currentTime().msecsSinceStartOfDay() / 1000.0;
    static double lastPointKey = 0;

    // 每1ms查询一次数据，如果有更新则增加一个数据
    if (key - lastPointKey > 0.001)
    {
        if (!fifo->isEmpty())
        {
            QVector<double> data = fifo->read();
            for (int var = 0; var < channelCount; ++var)
            {
                customPlot->graph(var)->addData(key, data[var]);
            }
        }
        lastPointKey = key;
    }

    // 显示8s的数据
    customPlot->xAxis->setRange(key, 8, Qt::AlignRight);
    // rpQueuedReplot表示不立即更新，使用该选项缩放和拖动窗口时曲线变形影响最小
    customPlot->replot(QCustomPlot::rpQueuedReplot);

#if SHOW_FPS_INFO
    static double lastFpsKey;
    static int frameCount;
    ++frameCount;
    if (key - lastFpsKey > 2)
    {
        qDebug() <<
                    QString("%1 FPS, Total Data points: %2")
                    .arg(frameCount / (key - lastFpsKey), 0, 'f', 0)
                    .arg(customPlot->graph(0)->data()->size() + customPlot->graph(1)->data()->size());

        lastFpsKey = key;
        frameCount = 0;
    }
#endif
}
