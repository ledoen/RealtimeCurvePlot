#ifndef DATAGENERATOR_H
#define DATAGENERATOR_H

#include <QObject>
#include <QThread>
#include "curveplotfifo.h"
#include <QTimer>


class DataGenerator : public QObject
{
    Q_OBJECT
public:
    DataGenerator(CurvePlotFifo* fifo, int channelCount, QObject *parent = nullptr);
private slots:
    void generateData();
private:
    CurvePlotFifo* fifo;
    int channelCount;
    QTimer dataTimer;
};

#endif // DATAGENERATORTHREAD_H
