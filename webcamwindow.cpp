#include "webcamwindow.h"
#include "ui_webcamwindow.h"

webcamwindow::webcamwindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::webcamwindow)
{
    ui->setupUi(this);
    setWindowTitle("Live webcam");
    webcamActivated = false;

}

webcamwindow::~webcamwindow()
{
    delete ui;
}

void webcamwindow::startFrameCapture()
{
    webcamActivated = true;
    capture.open(0);
    if (!capture.isOpened())
        {
            std::cout<<"can not open webcam bro";
            return;
        }
    connect(&timer, &QTimer::timeout, this, &webcamwindow::updateFrame);
    timer.start(33); // 30 FPS
}

void webcamwindow::stopFrameCapture()
{
    webcamActivated = false;
    timer.stop();
    capture.release();
}

void webcamwindow::updateFrame()
{
    if (webcamActivated)
    {
        cv::Mat frame;
        capture >> frame;

        if (frame.empty())
        {
            return;
        }

        // Convert the image format for displaying in the QLabel
        // cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);

        cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);

        // Create a QImage from the OpenCV frame
        // QImage qimage(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
        QImage qimage(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_Grayscale8);


        // Display the QImage in the QLabel
        ui->webcamlab->setPixmap(QPixmap::fromImage(qimage));
        ui->webcamlab->setScaledContents(true);
    }
}
