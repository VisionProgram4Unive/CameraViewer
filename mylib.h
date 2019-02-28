#ifndef MYLIB_H
#define MYLIB_H

namespace cv {
class Mat;
}

namespace m_impl {
    void histogramEqualize(cv::Mat & input, cv::Mat & output);
}
#endif // MYLIB_H
