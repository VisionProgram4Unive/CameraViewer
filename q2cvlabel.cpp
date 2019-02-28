#include "q2cvlabel.h"

#include <opencv2/core.hpp>
#include <QDebug>

Q2CVLabel::Q2CVLabel(QLabel *parent)
    : QLabel(parent)
{
    QRect geo({0,0}, parent->size());
    this->setGeometry(geo);
    this->setAlignment( Qt::AlignHCenter );

    for(int i=0;i<256;i++) colorTable.push_back( qRgb(i,i,i));
}

void Q2CVLabel::setMat(const cv::Mat &mat)
{
    auto mat2qimg = [&](const cv::Mat & mat){
        QImage img;
        switch (mat.type()) {
        case CV_8UC4:
            img = QImage(mat.data, mat.cols, mat.rows,
                         mat.step, QImage::Format_RGB32);
            break;
        case CV_8UC3:
            img = QImage(mat.data, mat.cols, mat.rows,
                         mat.step, QImage::Format_RGB888);
            img = img.rgbSwapped();
            break;
        case CV_8UC1:

            img = QImage(mat.data, mat.cols, mat.rows,
                         mat.step, QImage::Format_Indexed8);
            img.setColorTable(colorTable);
            break;
        default: ;
        }
        return img;
    };


    this->setPixmap( QPixmap::fromImage(mat2qimg(mat)).scaled(this->size(),
                     Qt::KeepAspectRatio) );
}
