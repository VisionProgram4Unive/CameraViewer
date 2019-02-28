#ifndef INPUTTHREAD_H
#define INPUTTHREAD_H

#include <QThread>
#include <QWaitCondition>
#include <QMutex>
#include <QImage>

#include "v4l2capture.h"

namespace cv {
class Mat;
}

class InputThread : public QThread
{
    Q_OBJECT
public:
    InputThread(QObject *parent = 0);
    ~InputThread();

	//camera
    void openCamera(std::string dev);
    void openCamera(int dev);
	//video
    void getData(std::string fileName);

    std::vector<Resolution> getResolustions();

signals:
    void updateMData(cv::Mat &);
//    void updateData(uchar data, int width, int height);
    void updateResolutionData(std::vector<Resolution> resolutionList);

public slots:
    void setCamera(int index);
    void run();

    void pause() { m_pause = true; }
    void resume() { m_pause = false; pauseCondition.wakeAll(); }
    void stop() { m_stop = true; resume(); }

private:
    void capture();
    QImage mat2qimage(cv::Mat &mat);

    unsigned long w_time;
	//camera values
    V4L2Capture *cap;
    uchar *bgr;
    unsigned int _width;
    unsigned int _height;
    std::string _dev;

    bool m_stop;
    bool m_pause;
    QWaitCondition pauseCondition;
    QMutex mutex;


    bool resetResolution;
};

#endif // INPUTTHREAD_H
