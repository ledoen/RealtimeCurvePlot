#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "datagenerator.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // 初始化缓冲区
    fifo = new CurvePlotFifo(1024);
    // 初始化数据生成线程
    DataGenerator* generator = new DataGenerator(fifo, CURVE_CHANNEL_COUNT);
    generator->moveToThread(&generatorThread);
    generatorThread.start();
    // 初始化曲线控件
    plot = new RealtimeCurvePlot(fifo, (int)CURVE_CHANNEL_COUNT);

    this->setCentralWidget(plot);
}

MainWindow::~MainWindow()
{
    delete ui;
}


