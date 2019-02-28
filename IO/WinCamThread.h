#ifndef WINCAMTHREAD_H
#define WINCAMTHREAD_H

#include "camthread.h"

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

/*
 * =====================================================================================
 *        Class:  WinCamThread
 *  Description:  Camera Thread for Windows System, wrapper of opencv video module..
 * =====================================================================================
 */

class WinCamThread : public CamThread
{
    Q_OBJECT
public:
    explicit WinCamThread(QObject *parent = nullptr);
    virtual ~WinCamThread() override;

    virtual void openCamera(int) override;
    virtual void openCamera(std::string) override;

    virtual void changeResolution(int) override;

private:
	cv::VideoCapture cap;
	cv::Mat frame;

	std::vector<Resolution> resolutions;

protected:
    virtual void doSomeWork() override;

};

#endif // WINCAMTHREAD_H
