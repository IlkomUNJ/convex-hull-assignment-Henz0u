#include "mainwindow.h"
#include <QApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();

    connect(slowButton, &QPushButton::clicked, this, &MainWindow::runSlowConvexHull);
    connect(fastButton, &QPushButton::clicked, this, &MainWindow::runFastConvexHull);
    connect(clearButton, &QPushButton::clicked, this, &MainWindow::clearCanvas);

    setWindowTitle("Convex Hull Assignment");
    resize(900, 700);
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    mainLayout = new QVBoxLayout(centralWidget);

    canvas = new DrawingWidget(this);
    canvas->setMinimumSize(700, 500);
    mainLayout->addWidget(canvas);

    buttonLayout = new QHBoxLayout();
    slowButton = new QPushButton("Run Slow Convex Hull", this);
    fastButton = new QPushButton("Run Fast Convex Hull", this);
    clearButton = new QPushButton("Clear Canvas", this);

    slowButton->setStyleSheet("background-color: #ff6b6b; color: white; padding: 8px; font-weight: bold;");
    fastButton->setStyleSheet("background-color: #4ecdc4; color: white; padding: 8px; font-weight: bold;");
    clearButton->setStyleSheet("background-color: #95a5a6; color: white; padding: 8px; font-weight: bold;");

    buttonLayout->addWidget(slowButton);
    buttonLayout->addWidget(fastButton);
    buttonLayout->addWidget(clearButton);

    mainLayout->addLayout(buttonLayout);
}

void MainWindow::runSlowConvexHull()
{
    canvas->runSlowConvexHull();
}

void MainWindow::runFastConvexHull()
{
    canvas->runFastConvexHull();
}

void MainWindow::clearCanvas()
{
    canvas->clearPoints();
}
