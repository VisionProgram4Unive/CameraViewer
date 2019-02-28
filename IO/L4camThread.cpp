#include "L4camThread.h"

//extern "C"{
#include <dirent.h>
//}

#include <QDebug>

L4CamThread::L4CamThread(QObject *parent)
    : CamThread(parent),
      cap(nullptr), bgr(nullptr)
{
    t_name += "-L4Cam";
}

L4CamThread::~L4CamThread()
{
    while(isRunning()) this->stop();

    delete cap;
    delete[] bgr;
}

void L4CamThread::openCamera(std::string dev)
{
    pause();

    cap = new V4L2Capture(dev.c_str());
    currentDev = dev;
    currentRes = cap->getResolutionList().at(0); // default resolution
    cap->setResolution(currentRes);

    emit updateResolutions( cap->getResolutionList() );

    isOpen = true;
}

void L4CamThread::openCamera(int dev)
{
    const std::string devLocation = "/dev";
    auto devName = "video" + std::to_string(dev);

    auto findDev = [&](){
        DIR *dp;
        struct dirent *dirp;

        dp=opendir(devLocation.c_str());
        while( (dirp=readdir(dp))!=NULL )
        {
            if(!strcmp(dirp->d_name, devName.c_str()))
                return true;
        }

        return false;
    };

    assert(findDev());

    openCamera(devLocation+"/"+devName);
}

void L4CamThread::changeResolution(int index)
{
    pause();

    //current device is opened than close
    if(isOpen) {
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

    qDebug() << t_name << ":: received Idx: "
             << currentRes.getWidth() << "*" << currentRes.getHeight() << endl;

    //re-allocate mem
    unsigned int height = cap->getCurrentResolution().getHeight();
    unsigned int width  = cap->getCurrentResolution().getWidth();
    bgr = new uchar[width*height*3];

    //pre heat camera buffers..
    for(int i=0;i<5;i++) cap->getBGRframe();

    resume();
}

void L4CamThread::doSomeWork()
{
    cap->getBGRframe(bgr);

    cv::Mat mat(currentRes.getHeight(), currentRes.getWidth(), CV_8UC3, bgr);
    emit updateMat(mat);
}
