#ifndef Q2CVLABEL_H
#define Q2CVLABEL_H

#include <QObject>
#include <QLabel>

namespace cv {
class Mat;
}

class Q2CVLabel : public QLabel
{
    Q_OBJECT
public:
    explicit Q2CVLabel(QLabel *parent = 0);

public slots:
    void setMat(const cv::Mat & mat);
private:
    QVector<QRgb> colorTable;
};

#endif // Q2CVLABEL_H
