#include "v4l2capture.h"

V4L2Capture::V4L2Capture()
    : type(V4L2_BUF_TYPE_VIDEO_CAPTURE),
      fileDesc(-1), openStatus(false), currentResolution(0,0), devName(NULL),
      num_buffers(2),
      yuyv(NULL), bgr(NULL)
{
}
V4L2Capture::V4L2Capture(const char *dev)
    : type(V4L2_BUF_TYPE_VIDEO_CAPTURE),
      fileDesc(-1), openStatus(false), currentResolution(0,0), devName(dev),
      num_buffers(2),
      yuyv(NULL), bgr(NULL)
{
    this->devOpen(dev);
}

V4L2Capture::V4L2Capture(const char *dev, size_t width, size_t height)
    : type(V4L2_BUF_TYPE_VIDEO_CAPTURE),
      fileDesc(-1), openStatus(false), currentResolution(0,0), devName(dev),
      num_buffers(2),
      yuyv(NULL), bgr(NULL)
{
    this->devOpen(dev);
    this->setResolution(width, height);
}

V4L2Capture::~V4L2Capture(){
    this->devClose();

    std::cout << "delete V4L2 OBJ" << std::endl;
}

void V4L2Capture::devOpen(const char *dev)
{
    if(isOpened()) this->devClose();

    fileDesc = open(dev, O_RDWR|O_NONBLOCK, 0);
    if(-1 == fileDesc) err_quit("cannot open device");

    this->openStatus = true;

    //query capcability
    struct v4l2_capability cap;
    if(-1 == xioctl(fileDesc, VIDIOC_QUERYCAP, &cap)) err_quit("vidioc_querycap");
    //if(!(cap.capabilities & v4l2_cap_video_capture))  err_quit("no support capture");
    //else if(!(cap.capabilities & v4l2_cap_streaming)) err_quit("no support streaming");

    //getting resolution lists
    struct v4l2_frmsizeenum frmsize;
    frmsize.pixel_format = V4L2_PIX_FMT_YUYV;
    frmsize.index		 = 0;
    while(xioctl(fileDesc, VIDIOC_ENUM_FRAMESIZES, &frmsize) >= 0){
        if(frmsize.type == V4L2_FRMSIZE_TYPE_DISCRETE)
        {
            resolutionList.push_back(
                        Resolution(frmsize.discrete.width, frmsize.discrete.height) );
        }else if(frmsize.type == V4L2_FRMSIZE_TYPE_STEPWISE)
        {
            resolutionList.push_back(
                        Resolution(frmsize.stepwise.max_width, frmsize.stepwise.max_height) );
        }
        frmsize.index++;
    }
}

void V4L2Capture::devClose()
{
    if(-1 == xioctl(fileDesc, VIDIOC_STREAMOFF, &type) )
        err_quit("VIDIOC_STREAMOFF");

    for(size_t i=0;i<num_buffers;++i)
        if(-1 == munmap(d_buffers[i].start, d_buffers[i].length))
            err_quit("munmap");

    free(d_buffers); d_buffers = NULL;
    free(yuyv); yuyv = NULL;
    free(bgr);  bgr  = NULL;

    this->openStatus = false;

    close(fileDesc);
}

void V4L2Capture::setResolution(const size_t width, const size_t height)
{
    if(currentResolution != Resolution(0,0)){
        this->devClose();
        this->devOpen(devName);
    }

    currentResolution = Resolution(width, height);
    yuyv = (uint8_t *)malloc(width*height*2*sizeof(uint8_t));
    bgr  = (uint8_t *)malloc(width*height*3*sizeof(uint8_t));

    CLEAR(fmt);
    fmt.type 				= V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width	 	= width;
    fmt.fmt.pix.height 		= height;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field 		= V4L2_FIELD_NONE;

    if(-1 == xioctl(fileDesc, VIDIOC_S_FMT, &fmt)) err_quit("set format");

    initBuffer();
}

void V4L2Capture::setResolution(const Resolution &resolution)
{
//    for(const Resolution & r : resolutionList){
//        if( r == resolution )
//            setResolution(resolution.getWidth(), resolution.getHeight());
//        else
//            err_quit("This Resolution is not supported");
//    }
    setResolution(resolution.getWidth(), resolution.getHeight());
}

void V4L2Capture::setResolution(const size_t listIdx)
{
    Resolution selectedResolution = resolutionList.at(listIdx);

    setResolution(selectedResolution.getWidth(), selectedResolution.getHeight());
}

const std::vector<Resolution> &V4L2Capture::getResolutionList() const
{
    return resolutionList;
}

void V4L2Capture::initBuffer()
{
    struct v4l2_requestbuffers req; CLEAR(req);
    req.count	= num_buffers;
    req.type 	= type; //V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory 	= V4L2_MEMORY_MMAP;
    if(-1 == xioctl(fileDesc, VIDIOC_REQBUFS, &req)) err_quit("Request Buffers");

    d_buffers = (buffer *)calloc(req.count, sizeof(*d_buffers));

    for(size_t i=0;i<req.count;++i){
        CLEAR(buf);
        buf.type 	= type;
        buf.memory 	= V4L2_MEMORY_MMAP;
        buf.index 	= i;
        if(-1 == xioctl(fileDesc, VIDIOC_QUERYBUF, &buf)) err_quit("Query Buffers");

        d_buffers[i].length = buf.length;
        d_buffers[i].start  =
            mmap(NULL, buf.length, PROT_READ|PROT_WRITE, MAP_SHARED,
                    fileDesc, buf.m.offset);
        if(MAP_FAILED == d_buffers[i].start) err_quit("mmap buffers");
    }

    for(size_t i=0;i<req.count;++i){
        CLEAR(buf);
        buf.type 	= type;
        buf.memory 	= V4L2_MEMORY_MMAP;
        buf.index = i;
        if(-1 == xioctl(fileDesc, VIDIOC_QBUF, &buf)) err_quit("Queue Buffers");
    }

    //camera ready!
    if(-1 == xioctl(fileDesc, VIDIOC_STREAMON, &buf.type)) err_quit("Stream on");
}

void V4L2Capture::capture(void *data, struct timeval timeout)
{
    if( currentResolution == Resolution(0,0) )
        err_quit("Device Resolution is not setted");
    if( !isOpened() )
        err_quit("Device is not opened");

    /* polling */
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(fileDesc, &fds);
    int r = select(fileDesc+1, &fds, NULL, NULL, &timeout);
    if(-1 == r) err_quit("Select");
    if( 0 == r) return ; // there's no frame?

    /* get data from dev buffer */
    CLEAR(buf);
    buf.type 	= type;
    buf.memory 	= V4L2_MEMORY_MMAP;
    if(-1 == xioctl(fileDesc, VIDIOC_DQBUF, &buf)) err_quit("Retriving Frame");
    memmove(data, d_buffers[buf.index].start, d_buffers[buf.index].length);
    //reload
    if(-1 == xioctl(fileDesc, VIDIOC_QBUF, &buf)) err_quit("Request Buffer Frame");
}

uint8_t *V4L2Capture::getBGRframe()
{
    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    capture(yuyv, timeout);
    yuyv2bgr(yuyv, bgr, fmt.fmt.pix.width, fmt.fmt.pix.height);

    return bgr;
}

void V4L2Capture::getBGRframe(void *data)
{
    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    capture(yuyv, timeout);
    yuyv2bgr(yuyv, (uint8_t *)data, fmt.fmt.pix.width, fmt.fmt.pix.height);
}
