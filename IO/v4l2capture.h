#ifndef V4L2CAPTURE_H
#define V4L2CAPTURE_H

//v4l2 headers..
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <linux/videodev2.h>
#include "libv4l2.h"

#include <iostream>
#include <vector>

#include "Resolution.h"

#define CLEAR(x) memset(&(x), 0, sizeof(x))

class V4L2Capture
{
public:
    /* constructor */
    V4L2Capture();
    V4L2Capture(const char *dev);
    V4L2Capture(const char *dev, size_t width, size_t height);
    /* destroyer */
    ~V4L2Capture();

    /* object setter */
    void devOpen(const char *dev);
    void setResolution(const size_t width, const size_t height);
    void setResolution(const Resolution & resolution);
    void setResolution(const size_t listIdx);

    /* object getter */
    uint8_t * getBGRframe();
    void getBGRframe(void *data);
    const std::vector<Resolution>& getResolutionList() const;
    const bool & isOpened() const { return openStatus; }
    const Resolution & getCurrentResolution() const { return currentResolution; }

private:
    //v4l2 values
    struct v4l2_format fmt;
    struct v4l2_buffer buf;
    enum v4l2_buf_type type;

    //common values
    int fileDesc;
    std::vector<Resolution> resolutionList;
    bool openStatus;
    Resolution currentResolution;
    const char * devName;

    // data value
    struct buffer{
        void * start;
        size_t length;
    } *d_buffers;
    size_t num_buffers;
    uint8_t *yuyv;
    uint8_t *bgr;

    //funcs
    int xioctl(int fd, int request, void *arg) {
        int r;
        do r = ioctl (fd, request, arg);
        while (-1 == r && EINTR == errno);

        return r;
    }
    void err_quit(const char *msg){
        fprintf(stderr, "[%s] %d: %s\n", msg, errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
    inline int bound(int min, int value, int max){
        return (value < min) ? min : (max < value) ? max : value;
    }
    inline void yuyv2bgr(uint8_t *yuyv, uint8_t *bgr, size_t width, size_t height){
        size_t i, j, index;
        int y0, y1, u, v;

        for(i=0;i<height;i++){
            for(j=0;j<width;j+=2){
                index = i * width + j;
                y0 = yuyv[index * 2 + 0] << 8;
                u  = yuyv[index * 2 + 1] - 128;
                y1 = yuyv[index * 2 + 2] << 8;
                v  = yuyv[index * 2 + 3] - 128;

                bgr[index * 3 + 0] = bound(0, (y0 + 454 * u) >> 8, 255);
                bgr[index * 3 + 1] = bound(0, (y0 + 88 * u - 183 * v) >> 8, 255);
                bgr[index * 3 + 2] = bound(0, (y0 + 359 * v) >> 8, 255);
                bgr[index * 3 + 3] = bound(0, (y1 + 454 * u) >> 8, 255);
                bgr[index * 3 + 4] = bound(0, (y1 + 88 * u - 183 * v) >> 8, 255);
                bgr[index * 3 + 5] = bound(0, (y1 + 359 * v) >> 8, 255);

            }
        }
    }

    void initBuffer();
    inline void capture(void *data, struct timeval timeout);

    void devClose();
};

#endif // V4L2CAPTURE_H
