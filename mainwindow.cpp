#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupLabels();

    webOBJ = new webcamwindow(this);

    connect(ui->BitPlane, &QPushButton::clicked, this, &MainWindow::onClicked);
    connect(ui->LIveBut, &QPushButton::clicked, this , &MainWindow::OpenWebcamWindow);
    connect(webOBJ, &webcamwindow::rejected, this, &MainWindow::closedWebcamWindow);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onClicked(){
QObject* ChosenBut = QObject::sender();

if (ChosenBut==ui->BitPlane){
    BitPlaneimages = GenerateBit();
    if (BitPlaneimages.empty()==true){
        qDebug() <<"vector empty";
        return ;
    }
    else{
        std::vector<cv::Mat>::iterator eachBitSlice;
        std::vector<QLabel*>::iterator eachLabel;
        eachLabel = BitPlaneLables.begin();
        for (eachBitSlice= BitPlaneimages.begin() ;eachBitSlice != BitPlaneimages.end() ; ++eachBitSlice, ++eachLabel ) {
            QLabel* label = *eachLabel;


            QImage convertedImage(eachBitSlice->data, eachBitSlice->cols, eachBitSlice->rows, eachBitSlice->step, QImage::Format_Grayscale8);

            label->setPixmap(QPixmap::fromImage(convertedImage));
//            cv::imshow("bit",*eachBitSlice);
//            cv::waitKey(0);
        }
    }
}

else
    qDebug() <<"not clicekd";

}

void MainWindow::setupLabels(){
    BitPlaneLables.push_back(ui->bitPlane1);
    BitPlaneLables.push_back(ui->bitPlane2);
    BitPlaneLables.push_back(ui->bitPlane3);
    BitPlaneLables.push_back(ui->bitPlane4);
    BitPlaneLables.push_back(ui->bitPlane5);
    BitPlaneLables.push_back(ui->bitPlane6);
    BitPlaneLables.push_back(ui->bitPlane7);
    BitPlaneLables.push_back(ui->bitPlane8);

    for (auto label : BitPlaneLables) {
        label->setScaledContents(true);
    }
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

//void MainWindow::updateFrame()
//{
//    cv::Mat frame;
//    capture >> frame;
//    if (frame.empty())
//        return;

//    // Convert the image format for processing
//    cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);

//    cv::Mat bitPlanes[8];
//       for (int plane = 0; plane < 8; ++plane) {
//           bitPlanes[plane] = cv::Mat(frame.rows, frame.cols, CV_8UC1);
//           for (int i = 0; i < frame.rows; ++i) {
//               for (int j = 0; j < frame.cols; ++j) {
//                   bitPlanes[plane].at<uchar>(i, j) = (frame.at<uchar>(i, j) >> plane) & 1;
//                   bitPlanes[plane].at<uchar>(i, j) *= 255;
//               }
//           }
////           displayImage(bitPlanes[plane]);
//       }

//    // Create a QImage from the OpenCV frame
//    QImage qimage(redFrame.data, redFrame.cols, redFrame.rows, redFrame.step, QImage::Format_RGB888);

//    // Display the QImage in the QLabel
//    ui->webcamlab->setPixmap(QPixmap::fromImage(qimage));
//    ui->webcamlab->setScaledContents(true);
//}

void MainWindow::OpenWebcamWindow(){
    ui->BitPlane->setEnabled(false);
    webOBJ->show();
    webOBJ->startFrameCapture();
}
void MainWindow::closedWebcamWindow(){
    webOBJ->stopFrameCapture();
    ui->BitPlane->setEnabled(true);
}
