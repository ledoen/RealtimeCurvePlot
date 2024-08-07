#include "datagenerator.h"
#include <QDebug>

DataGenerator::DataGenerator(CurvePlotFifo* fifo, int channelCount, QObject *parent) : QObject(parent), fifo(fifo), channelCount(channelCount)
{
    connect(&dataTimer, SIGNAL(timeout()), this, SLOT(generateData()));
    dataTimer.start(5);
}

void DataGenerator::generateData()
{
    static QElapsedTimer timer;
    QVector<double> newData;
    int i;
    double key = timer.elapsed() * 2 / 1000.0;
    for (i = 0; i < channelCount; i++)
    {
        newData.append(qSin(key + i * M_PI / 2));
    }

    if (!fifo->isFull())
    {
        fifo->write(newData);
    }
}
