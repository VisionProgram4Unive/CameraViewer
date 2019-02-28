#include "pausablethread.h"

#include <QDebug>

PausableThread::PausableThread(QObject *parent)
    : QThread(parent),
      m_stop(false), m_pause(true), interval(0),
      t_name("PausableThread")
{

}

PausableThread::~PausableThread()
{

}

void PausableThread::run()
{
    while(1){
        if(m_stop){
            break;
        } else if(m_pause){
            mutex.lock();
            qDebug() << t_name << ":: Thread Waiting " << endl;
            pauseCond.wait(&mutex);
            mutex.unlock();
        }

        //      running statment entry
        if(m_stop || m_pause) continue;
//        qDebug() << "updating Thread Running" << endl;

        doSomeWork();

        msleep(interval);
    }
    qDebug() <<t_name<< ":: Thread Stoped" << endl;
}

void PausableThread::doSomeWork()
{
    qDebug() <<t_name<< ":: I do Something, anyway.." << endl;
}
