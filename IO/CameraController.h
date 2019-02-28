#ifndef  CAMERACONTROLLER_H_INC
#define  CAMERACONTROLLER_H_INC

/*
 * =====================================================================================
 *        Class:  CameraController
 *  Description:  Depend on A OS, Camera control class based on QThread
 *  			  This are organized into those classes..
 *  			  QThread \
 *  			  		   -> PausableThread \
 *  			  		   				      -> CamThread \
 *  			  		   				                    -> L4CamThread,
 *  			  		   				                    -> WinCamThread
 * =====================================================================================
 */

#ifdef  __gnu_linux__
    #include "L4camThread.h"
class CameraController : public L4CamThread
{
    Q_OBJECT
    public:
        explicit CameraController (QObject * parent = nullptr)
            : L4CamThread(parent)
    {}

#elif _WIN32      /* -----  Windows system  ----- */
    #include "WinCamThread.h"
class CameraController : public WinCamThread
{
    Q_OBJECT
    public:
        explicit CameraController (QObject * parent = 0)
            : WinCamThread(parent)
    {}

#endif

    virtual ~CameraController() override {}

    protected:

    private:

}; /* -----  end of class CameraController  ----- */
#endif   /* ----- #ifndef CAMERACONTROLLER_H_INC  ----- */

