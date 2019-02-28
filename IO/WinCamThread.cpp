#include "WinCamThread.h"

#include <QDebug>


WinCamThread::WinCamThread(QObject *parent)
    : CamThread(parent),
      resolutions{ {640,480}, {320,240},  {800,600},
                   {960,720}, {1280,720}, {1600,900} }
{
    currentRes = {640, 480};

	t_name += "-WinCam";
}

WinCamThread::~WinCamThread()
{
	while(isRunning()) this->stop();
}

void WinCamThread::openCamera(int dev)
{
	pause();

	currentDev = dev;
	if(!cap.open(dev)) qDebug() << "Can not find Camera device!" << endl;

    isOpen = true;

    cap.set(CV_CAP_PROP_FRAME_WIDTH,  640 );
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 480 );

    emit updateResolutions( resolutions );
}

void WinCamThread::openCamera(std::string)
{
    qDebug() << t_name << "this method is not implemented" << endl;
}

void WinCamThread::changeResolution(int index)
{
	pause();

	currentRes = resolutions.at(index);
	
	qDebug() << t_name << ":: received Idx: "
			 << currentRes.getWidth() << "*" << currentRes.getHeight() << endl;

    auto fr_query = [this](){
        bool wid = cap.set(CV_CAP_PROP_FRAME_WIDTH, static_cast<double>(currentRes.getWidth()));
        bool hei = cap.set(CV_CAP_PROP_FRAME_HEIGHT, static_cast<double>(currentRes.getHeight()));

        if(!wid || !hei)
            return false;
        else
            return true;
    };

    if(!fr_query()){
        qDebug() << t_name << ":: this resolution is not support on this device, "
                           << "set a default resolution." << endl;

        currentRes = resolutions.at(0);

        cap.release();
        cap.open(currentDev);

        cap.set(CV_CAP_PROP_FRAME_WIDTH,  640 );
        cap.set(CV_CAP_PROP_FRAME_HEIGHT, 480 );
    }

	resume();
}

void WinCamThread::doSomeWork()
{
	cap >> frame;

	emit updateMat(frame);
}
