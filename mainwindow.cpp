#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QDebug>
//#include <opencv2/opencv.hpp>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->openbut, &QPushButton::clicked, this, &MainWindow::onClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onClicked(){
QObject* but = QObject::sender();

if (but==ui->openbut){
    GenerateBit();
    if (BitPlaneimages.empty()==true){
        qDebug() <<"vector empty";
        return ;
    }
    else
        qDebug() <<"vector full";
}

else
    qDebug() <<"not clicekd";

}

std::vector<cv::Mat> MainWindow::GenerateBit()
{
    // Open file dialog to choose an image
    QString imagePath = QFileDialog::getOpenFileName(this, "Open Image", "", "Image Files (*.png *.jpg *.jpeg)");

    // Check if a valid image was selected
    if (imagePath.isEmpty()) {
        qDebug() << "No image selected.";
        return std::vector<cv::Mat>(); // Return an empty vector
    }

    // Load the image using OpenCV
    cv::Mat image = cv::imread(imagePath.toStdString(), cv::IMREAD_GRAYSCALE);

    // Check if the image was loaded successfully
    if (image.empty()) {
        qDebug() << "Failed to load image!";
        return std::vector<cv::Mat>(); // Return an empty vector
    }

    // Perform bit plane slicing
    if (!BitPlaneimages.empty())
        BitPlaneimages.clear();

    for (int plane = 0; plane < 8; ++plane) {
        cv::Mat slicedImage(image.rows, image.cols, CV_8UC1);
        for (int i = 0; i < image.rows; ++i) {
            for (int j = 0; j < image.cols; ++j) {
                slicedImage.at<uchar>(i, j) = (image.at<uchar>(i, j) >> plane) & 1;
                slicedImage.at<uchar>(i, j) *= 255;
            }
        }
        BitPlaneimages.push_back(slicedImage);
    }

    return BitPlaneimages;
}
