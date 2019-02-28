#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "q2cvlabel.h"
#include "IO/CameraController.h"

#include <opencv2/core.hpp>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

private:
    Ui::MainWindow *ui;

    Q2CVLabel * view;
    cv::Mat buffer;

    int g_value;

    CameraController *vidInput;

    void process(cv::Mat &);
};

#endif // MAINWINDOW_H
