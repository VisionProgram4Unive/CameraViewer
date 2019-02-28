#ifndef L4CAMTHREAD_H
#define L4CAMTHREAD_H

#include "camthread.h"
#include "v4l2capture.h"

#include <opencv2/core.hpp>

/*
 * =====================================================================================
 *        Class:  L4CamThread
 *  Description:  Camera Thread for Linux System, it based on a v4l library.
 *  			  See the v4l2capture.h for detail...
 * =====================================================================================
 */
class L4CamThread : public CamThread
{
    Q_OBJECT
public:
    explicit L4CamThread(QObject *parent = nullptr);
    virtual ~L4CamThread() override;

    virtual void openCamera(int) override;
    virtual void openCamera(std::string) override;

    virtual void changeResolution(int) override;

signals:
    void updateMat(cv::Mat &);
    void updateResolutions(std::vector<Resolution>);

private:
    V4L2Capture * cap;
	uchar * bgr;

    //std::string currentDev;
    //bool isOpen;

protected:
    virtual void doSomeWork() override;

};

#endif // L4CAMTHREAD_H
