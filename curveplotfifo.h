#ifndef CURVEPLOTFIFO_H
#define CURVEPLOTFIFO_H

#include <QObject>
#include <QPair>
#include <QMutex>
#include <QQueue>

class CurvePlotFifo : QObject
{
    Q_OBJECT
public:
    CurvePlotFifo(int capacity);
    QVector<double> read();
    void write(QVector<double> data);
    bool isEmpty() const;
    bool isFull() const;
    void clear();

private:
    int capacity;
    mutable QMutex mutex;
    QQueue<QVector<double>> buffer;
};

#endif // CURVEPLOTFIFO_H
