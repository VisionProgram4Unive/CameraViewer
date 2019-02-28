#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <opencv2/opencv.hpp>

#include "mylib.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    g_value(0)
{
    ui->setupUi(this);

    view = new Q2CVLabel(ui->label);

    vidInput = new CameraController(this);
    vidInput->start();
    connect(vidInput, &CameraController::updateMat,
            [&](cv::Mat & image){
        process(image);

        view->setMat(buffer);
    });
    connect(vidInput, &CameraController::updateResolutions,
            [&](std::vector<Resolution> resolutions){
        ui->comboBox->clear();
        for(auto && resolution : resolutions){
            QString newResolution =
                QString::number(resolution.getWidth()) + "*" +
                QString::number(resolution.getHeight());
            ui->comboBox->addItem(newResolution);
        }
    });
    connect(ui->pushButton, &QPushButton::clicked,
            [&](){
        vidInput->changeResolution( ui->comboBox->currentIndex() );
    });

//    connect(ui->spinBox, &QSpinBox::valueChanged,
//            [&](int value){
//        g_value = value;
//    });
    connect(ui->horizontalSlider, &QSlider::valueChanged,
            [&](int value){
        g_value = value;
    });
    connect(ui->comboBox_2, &QComboBox::currentTextChanged,
            [&](){
        const QString al_name = ui->comboBox_2->currentText();

        if("Brightness" == al_name||"Contrast" == al_name||"Saturation" == al_name)
            ui->horizontalSlider->setValue(5);

        if("NULL" == al_name || "Histogram Equalize" == al_name) {
            ui->horizontalSlider->setEnabled(false);
            ui->spinBox->setEnabled(false);
        } else {
            ui->horizontalSlider->setEnabled(true);
            ui->spinBox->setEnabled(true);
        }
    });

    vidInput->openCamera(0);
}

MainWindow::~MainWindow()
{
    delete vidInput;

    delete view;

    delete ui;
}

void MainWindow::process(cv::Mat& mat)
{
    const QString al_name = ui->comboBox_2->currentText();

    if("NULL" == al_name)
        mat.copyTo(buffer);

    else if("Blurring" == al_name)
        cv::medianBlur(mat, buffer, g_value*2+1);

    else if("BilateralFilter" == al_name)
        cv::bilateralFilter(mat, buffer, 5, 60*g_value+1, 60*g_value+1);

    else if("Brightness" == al_name)
        [&](double beta){
        mat.convertTo(buffer, -1, 1, beta);
    } ( static_cast<double>(g_value-5)*10 );

    else if("Contrast" == al_name)
        [&](float delta){

        double alpha = 255.0/(255.0-delta*2);
        double beta  = -alpha;

        mat.convertTo(buffer, -1, alpha, beta);
    }( 127.0f * static_cast<float>(g_value-5)/10.0f );

    else if("Sharpening" == al_name)
        [&](float alpha){
        cv::Mat smoothed;
        cv::medianBlur(mat, smoothed, 17);
        buffer = alpha * (mat-smoothed) + mat;
    }( 1.0f + static_cast<float>(g_value)/10.f );

    else if("Histogram Equalize" == al_name)
//        m_impl::histogramEqualize(mat, buffer);
        [&](){
        cv::Mat hls;
        cv::cvtColor(mat, hls, CV_BGR2HLS);

        std::vector<cv::Mat> channel;
        cv::split(hls, channel);

        m_impl::histogramEqualize(channel[1], channel[1]);

        cv::merge(channel, buffer);
        cv::cvtColor(buffer, buffer, CV_HLS2BGR);
    };

    else if("Saturation" == al_name)
        [&](float value){
        cv::Mat hsv;
        cv::cvtColor(mat, hsv, CV_BGR2HSV);

        for(int j=0;j<mat.rows;j++)
        {
            uchar * pSat = hsv.ptr<uchar>(j);
            for(int i=0;i<mat.cols;i++)
            {
                pSat[i*3+1] = cv::saturate_cast<uchar>(pSat[i*3+1] + 25*value);
            }
        }

        cv::cvtColor(hsv, buffer, CV_HSV2BGR);
    }( static_cast<float>(g_value-5) );


}
