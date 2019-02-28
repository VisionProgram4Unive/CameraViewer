#include "inputthread.h"
#include <QDebug>
#include <opencv2/opencv.hpp>

InputThread::InputThread(QObject *parent)
    : QThread(parent),
      w_time(30),
      cap(nullptr), bgr(nullptr),
      _width(0),_height(0),
      m_stop(false), m_pause(true),
      resetResolution(false)
{
    qRegisterMetaType<cv::Mat>("cv::Mat");
}

InputThread::~InputThread()
{
    while(isRunning()) stop();

    delete cap; cap = NULL;
    delete[] bgr; bgr = NULL;
}

/* To getting resolution List from temp capture obj  */
void InputThread::openCamera(std::string dev)
{
    pause();

    cap = new V4L2Capture(dev.c_str());
    _dev = dev;
    Resolution default_res = cap->getResolutionList().at(0);
    _width = default_res.getWidth(); _height = default_res.getHeight();
    cap->setResolution(_width, _height);

    emit updateResolutionData( cap->getResolutionList() );

    resetResolution = true;

    bgr = new uchar[_width*_height*3];

    //pre heat camera buffers..
    for(int i=0;i<5;i++) cap->getBGRframe();

}

void InputThread::openCamera(int dev)
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

void InputThread::setCamera(int index)
{
    pause();

    //current device is opened than close
    if(this->resetResolution)
    {
        delete cap;
        delete[] bgr;

        qDebug() << "Force Sleep" << endl;
        this->sleep( 3 );
    }

    cap = new V4L2Capture(_dev.c_str());
    Resolution resolution = cap->getResolutionList().at(index);
    _width = resolution.getWidth(); _height = resolution.getHeight();
    cap->setResolution(_width, _height);

    qDebug() << "received index : " << index << endl;
    qDebug() << _width << "*" << _height << endl;

    //reallocate mem
    bgr = new uchar[_width*_height*3];

    //pre heat camera buffers..
    for(int i=0;i<5;i++) cap->getBGRframe();


//    V4L2Capture tempCap(_dev.c_str());
//    emit updateResolutionData( tempCap.getResolutionList() );
//    Resolution defResolution = tempCap.getResolutionList().at(0);
//    tempCap.setResolution(defResolution.getWidth(), defResolution.getHeight());

    resume();
}

void InputThread::getData(std::string fileName)
{
    pause();

    cv::Mat image = cv::imread(fileName);
    emit updateMData( image );
}

std::vector<Resolution> InputThread::getResolustions()
{
    return cap->getResolutionList();
}

void InputThread::run()
{
    while(1){
        if(m_stop){
            break;
        } else if(m_pause){
            mutex.lock();
            qDebug() << "INPUT Thread Waiting " << endl;
            pauseCondition.wait(&mutex);
            mutex.unlock();
        }

        //      running statment entry
        if(m_stop || m_pause) continue;
//        qDebug() << "updating Thread Running" << endl;
        capture();
        msleep(w_time);
    }
    qDebug() << "INPUT Thread Stoped" << endl;
}

void InputThread::capture()
{
    cap->getBGRframe(bgr);

    cv::Mat buffer = cv::Mat(_height, _width, CV_8UC3, bgr);

    emit updateMData(buffer);
}

//void InputThread::yuyv2bgr(uchar *yuyv, uchar *bgr, unsigned int width, unsigned int height)
//{
//    auto bound = [](int min, int value, int max)
//    {
//        return (value < min) ? min : (max < value) ? max : value;
//    };

//    for (size_t i = 0; i < height; i++)   {
//    for (size_t j = 0; j < width; j += 2) {
//        size_t index = i * width + j;
//        int y0 = yuyv[index * 2 + 0] << 8;
//        int u = yuyv[index * 2 + 1] - 128;
//        int y1 = yuyv[index * 2 + 2] << 8;
//        int v = yuyv[index * 2 + 3] - 128;

//        bgr[index * 3 + 0] = bound(0, (y0 + 454 * u) >> 8, 255);
//        bgr[index * 3 + 1] = bound(0, (y0 + 88 * u - 183 * v) >> 8, 255);
//        bgr[index * 3 + 2] = bound(0, (y0 + 359 * v) >> 8, 255);
//        bgr[index * 3 + 3] = bound(0, (y1 + 454 * u) >> 8, 255);
//        bgr[index * 3 + 4] = bound(0, (y1 + 88 * u - 183 * v) >> 8, 255);
//        bgr[index * 3 + 5] = bound(0, (y1 + 359 * v) >> 8, 255);
//    }
//    }
//}
