#ifndef CAMTHREAD_H
#define CAMTHREAD_H

#include "pausablethread.h"
#include "Resolution.h"

namespace cv{
	class Mat;
}

/*
 * =====================================================================================
 *        Class:  Thread For Camera interface
 *  Description:  To make a multi-platform interface, 
 *  			  extract a common interface of camera controller
 * =====================================================================================
 */
class CamThread : public PausableThread
{
    Q_OBJECT
public:
    explicit CamThread(QObject *parent = nullptr)
		: PausableThread(parent),
		  isOpen(false)
	{
		interval = 30;
		t_name += ":Camera";
	}

    ~CamThread() {}

    virtual void openCamera(int) = 0;
    virtual void openCamera(std::string) = 0;

    virtual void changeResolution(int) = 0;

    const Resolution &currentResolution() const{
        return currentRes;
    }

signals:
    void updateMat(cv::Mat &);
    void updateResolutions(std::vector<Resolution>);

protected:
    std::string currentDev;
    bool isOpen;

	Resolution currentRes;
};

#endif // CAMTHREAD_H
