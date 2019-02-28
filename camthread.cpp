#include "camthread.h"

#include <QDebug>

#include <opencv2/core.hpp>

CamThread::CamThread(QObject *parent)
    : PausableThread(parent),
      cap(nullptr), bgr(nullptr), isOpen(false)
{
    interval = 30;
    t_name += ":Camera";
}

CamThread::~CamThread()
{
    while(isRunning()) this->stop();

    delete cap;
    delete[] bgr;
}

void CamThread::openCamera(std::string dev)
{
    pause();

    cap = new V4L2Capture(dev.c_str());
    currentDev = dev;
    currentRes = cap->getResolutionList().at(0); // default resolution
    cap->setResolution(currentRes);

    emit updateResolutions( cap->getResolutionList() );

    isOpen = true;
}

void CamThread::openCamera(int dev)
{
    switch(dev)
    {
    case 0:
        openCamera("/dev/video0");
        break;
    case 1:
        openCamera("/dev/video1");
        break;
    case 2:
        openCamera("/dev/video2");
        break;
    default:
        qDebug() << "Can not find Camera device!" << endl;
    }
}

void CamThread::changeResolution(int index)
{
    pause();

    //current device is opened than close
    if(isOpen)
    {
        qDebug() << t_name << ":: Force Sleep" << endl;

        delete cap;
        delete[] bgr;

        this->sleep( 2 );

        cap = new V4L2Capture(currentDev.c_str());
    } else {
        qDebug() << t_name << ":: camera isn't opened, set a default cam" << endl;
        openCamera(0);
    }

    currentRes = cap->getResolutionList().at(index);
    cap->setResolution(currentRes);

    unsigned int height = cap->getCurrentResolution().getHeight();
    unsigned int width  = cap->getCurrentResolution().getWidth();

    qDebug() << t_name << ":: received Idx: "
             << currentRes.getWidth() << "*" << currentRes.getHeight() << endl;

    //reallocate mem
    bgr = new uchar[width*height*3];

    //pre heat camera buffers..
    for(int i=0;i<5;i++) cap->getBGRframe();

    resume();
}

void CamThread::doSomeWork()
{
    cap->getBGRframe(bgr);

    cv::Mat mat(currentRes.getHeight(), currentRes.getWidth(), CV_8UC3, bgr);
    emit updateMat(mat);
}
