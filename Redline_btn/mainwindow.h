#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QWidget>
#include <QLabel>
#include "drawingwidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void runSlowConvexHull();
    void runFastConvexHull();
    void clearCanvas();

private:
    void setupUI();

    QWidget *centralWidget;
    QVBoxLayout *mainLayout;
    DrawingWidget *canvas;
    QHBoxLayout *buttonLayout;
    QPushButton *slowButton;
    QPushButton *fastButton;
    QPushButton *clearButton;
    QLabel *instructionLabel;
};

#endif // MAINWINDOW_H
