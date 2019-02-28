#include "mylib.h"

extern "C" {
#include "MY/histogram_equalization.h"
}

#include <opencv2/core.hpp>

#include <opencv2/imgproc.hpp>

using u8 = unsigned char;

void m_impl::histogramEqualize(cv::Mat &input, cv::Mat &output)
{
    switch (input.type()) {
    case CV_8UC1:
        break;
    case CV_8UC3:
    case CV_8UC4:
        cv::cvtColor(input, input, CV_RGB2GRAY);
        break;
    default:
        cv::error(cv::Exception( CV_StsAssert, "can't be convert input matrix..", "histogramEqualize", "", 24 ));
    }

//    cv::Mat diff = (input!=output);
//    bool eq = (cv::countNonZero(diff) == 0);
//    if(!eq)
    output = input.clone();

    u8 * pInput  = input.ptr<u8>(0, 0);
    u8 * pOutput = output.ptr<u8>(0, 0);
//    u8 * pInput  = input.data;
//    u8 * pOutput = output.data;

    histogram_equalization(pInput, pOutput, input.cols, input.rows);
}
