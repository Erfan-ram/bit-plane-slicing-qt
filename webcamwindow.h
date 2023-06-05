#ifndef WEBCAMWINDOW_H
#define WEBCAMWINDOW_H

#include <QDialog>
#include <QTimer>
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

private:
    Ui::webcamwindow *ui;
    cv::VideoCapture capture;
    QTimer timer;
    bool webcamActivated;
};

#endif // WEBCAMWINDOW_H
