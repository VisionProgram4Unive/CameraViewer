#ifndef PAUSABLETHREAD_H
#define PAUSABLETHREAD_H

#include <QThread>
#include <QWaitCondition>
#include <QMutex>

class PausableThread : public QThread
{
    Q_OBJECT
public:
    explicit PausableThread(QObject *parent = 0);
    ~PausableThread();

    void pause() { m_pause = true; }
    void resume() { m_pause = false; pauseCond.wakeAll(); }
    void stop() { m_stop = true; resume(); }

public slots:
    void run();
protected:
    bool m_stop;
    bool m_pause;
    unsigned long interval;
    QWaitCondition pauseCond;
    QMutex mutex;

    QString t_name;

protected:
    virtual void doSomeWork();
};

#endif // PAUSABLETHREAD_H
