#ifndef WEBCAMWINDOW_H
#define WEBCAMWINDOW_H

#include <QDialog>
#include <QTimer>
#include <QScrollBar>
#include <opencv2/opencv.hpp>

namespace Ui {
class webcamwindow;
}

class webcamwindow : public QDialog
{
    Q_OBJECT

public:
    explicit webcamwindow(QWidget *parent = nullptr);
    ~webcamwindow();
    void startFrameCapture();
    void stopFrameCapture();

private slots:
    void updateFrame();
    void setBitPosition(int value);

private:
    Ui::webcamwindow *ui;
    cv::VideoCapture capture;
    QTimer timer;
    bool webcamActivated;
    int BitPosition;
    QScrollBar *bitScrollBar;
};

#endif // WEBCAMWINDOW_H
