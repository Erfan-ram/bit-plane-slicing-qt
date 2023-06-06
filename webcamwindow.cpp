#include "webcamwindow.h"
#include "ui_webcamwindow.h"

#include<QDebug>

webcamwindow::webcamwindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::webcamwindow)
{
    ui->setupUi(this);
    setWindowTitle("Live webcam");
    webcamActivated = false;

    BitPosition = 7;
    ui->bitScrollBar->setValue(BitPosition);

    connect(ui->bitScrollBar,&QScrollBar::valueChanged,this,&webcamwindow::setBitPosition);

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

        cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);

        //**** 1-8th-bitplane
        cv::Mat slicedImage = cv::Mat::zeros(frame.size(), CV_8UC1);

           for (int i = 0; i < frame.rows; i++)
           {
               for (int j = 0; j < frame.cols; j++)
               {
                   // Extract the bit at the specified plane
                   slicedImage.at<uchar>(i, j) = (frame.at<uchar>(i, j) >> BitPosition) & 1;

                   // Optionally, scale the pixel value to 0-255 for visualization
                   slicedImage.at<uchar>(i, j) *= 255;
               }
           }
        //*******

        // Create a QImage from the OpenCV frame
        QImage qimage(slicedImage.data, slicedImage.cols, slicedImage.rows, slicedImage.step, QImage::Format_Grayscale8);

        // Display the QImage in the QLabel
        ui->webcamlab->setPixmap(QPixmap::fromImage(qimage));
        ui->webcamlab->setScaledContents(true);
    }
}

void webcamwindow::setBitPosition(int value){
    BitPosition = value;
    qDebug() <<value;
}
