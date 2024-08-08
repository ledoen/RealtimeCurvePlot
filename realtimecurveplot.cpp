#include "realtimecurveplot.h"
#define SHOW_FPS_INFO 0

RealtimeCurvePlot::RealtimeCurvePlot(CurvePlotFifo* fifo, int channelCount, QWidget *parent)
    : QWidget{parent}
    , fifo(fifo)
    , channelCount(channelCount)
{
    initLayout();
    initDisplayLengthComboBox();
    initCurvePlot();

    connect(&replotTimer, SIGNAL(timeout()), this, SLOT(refreshCurve()));
    replotTimer.start(0);
}

/// 初始化plot，配置曲线数，坐标轴等
void RealtimeCurvePlot::initCurvePlot()
{
    // 根据传入的曲线个数，创建曲线
    for (int var = 0; var < channelCount; ++var)
    {
        customPlot->addGraph();
        // 设置曲线颜色
        QColor color;
        color.setHsl(var * 75 % 360, 255, 96);
        customPlot->graph(var)->setPen(color);

        // 设置曲线名称
        QString curveName;
        if (var == 0)
        {
            curveName = "指令";
        }
        else
            curveName = "通道" + QString::number(var);
        customPlot->graph(var)->setName(curveName);
    }

    // 关闭拖动时抗锯齿，提高拖动时性能，避免拖动窗口时曲线变形
    customPlot->setNoAntialiasingOnDrag(true);

    // 设置坐标轴
    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    customPlot->xAxis->setTicker(timeTicker);
    customPlot->axisRect()->setupFullAxesBox();
    customPlot->yAxis->setRange(-1.2, 1.2);

    // 设置legend
    customPlot->legend->setVisible(true);
    customPlot->legend->setSelectableParts(QCPLegend::spItems);
    customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft | Qt::AlignTop);


    // 设置legend被点击事件
    connect(customPlot, SIGNAL(legendClick(QCPLegend*,QCPAbstractLegendItem*,QMouseEvent*)), this, SLOT(onLegendClick(QCPLegend*,QCPAbstractLegendItem*,QMouseEvent*)));

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
    customPlot->xAxis->setRange(key, displayLengthInSecond, Qt::AlignRight);
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

void RealtimeCurvePlot::onLegendClick(QCPLegend *legend, QCPAbstractLegendItem *item, QMouseEvent *event)
{
    // 将抽象图例项转换为具体的图例项类型
    QCPPlottableLegendItem *plItem = qobject_cast<QCPPlottableLegendItem*>(item);
    if (plItem)
    {
        // 获取被点击的图例对应的曲线
        QCPGraph* graph = qobject_cast<QCPGraph*>(plItem->plottable());
        if (graph)
        {
            // 切换曲线的可见性
            graph->setVisible(!graph->visible());

            // 设置切换可见性曲线对应的legend字体颜色
            plItem->setTextColor(graph->visible() ? Qt::black : Qt::gray);
        }
    }
}

void RealtimeCurvePlot::initDisplayLengthComboBox()
{
    displayLenthComboBox->addItem("1s",  1);
    displayLenthComboBox->addItem("2s",  2);
    displayLenthComboBox->addItem("5s",  5);
    displayLenthComboBox->addItem("10s", 10);
    displayLenthComboBox->addItem("20s", 20);
    displayLenthComboBox->addItem("40s", 40);
    displayLenthComboBox->addItem("60s", 60);
    // 默认设置为5s
    displayLenthComboBox->setCurrentIndex(2);

    connect(displayLenthComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onDisplayLengthChanged(int)));
}

void RealtimeCurvePlot::initLayout()
{
    customPlot = new QCustomPlot();

    // 总布局是一个垂直布局
    QVBoxLayout* layout = new QVBoxLayout(this);

    //上层layout
    QHBoxLayout* displayLengthLayout = new QHBoxLayout();
    QLabel* displayLengthLabel = new QLabel();
    displayLengthLabel->setText("时间长度");
    displayLengthLayout->addWidget(displayLengthLabel);
    displayLenthComboBox = new QComboBox();
    displayLengthLayout->addWidget(displayLenthComboBox);
    displayLengthLayout->addStretch();

    // 总layout
    layout->addLayout(displayLengthLayout);
    layout->addWidget(customPlot);
}

void RealtimeCurvePlot::onDisplayLengthChanged(int index)
{
    displayLengthInSecond = displayLenthComboBox->itemData(index).toInt();
}
