#ifndef CAMTHREAD_H
#define CAMTHREAD_H

#include "pausablethread.h"
#include "IO/v4l2capture.h"

//#include <opencv2/core.hpp>
namespace cv {
class Mat;
}

class CamThread : public PausableThread
{
    Q_OBJECT
public:
    explicit CamThread(QObject *parent = 0);
    ~CamThread();

    void openCamera(int);
    void openCamera(std::string);

    void changeResolution(int);

signals:
    void updateMat(cv::Mat &);
    void updateResolutions(std::vector<Resolution>);

private:
    V4L2Capture * cap;
    uchar * bgr;

    std::string currentDev;
    bool isOpen;

    Resolution currentRes;

protected:
    virtual void doSomeWork() override;

};

#endif // CAMTHREAD_H
