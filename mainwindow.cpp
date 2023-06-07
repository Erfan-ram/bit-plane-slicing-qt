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
            cv::imshow("bit",*eachBitSlice);
            cv::waitKey(0);
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
    QString imagePath = QFileDialog::getOpenFileName(this, "Open Image", "", "Image Files (*.png *.jpg *.jpeg)");

    if (imagePath.isEmpty()) {
        qDebug() << "No image selected.";
        return std::vector<cv::Mat>();
    }

    cv::Mat image = cv::imread(imagePath.toStdString(), cv::IMREAD_GRAYSCALE);

    if (image.empty()) {
        qDebug() << "Failed to load image!";
        return std::vector<cv::Mat>();
    }

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

void MainWindow::OpenWebcamWindow(){
    ui->BitPlane->setEnabled(false);
    webOBJ->show();
    webOBJ->startFrameCapture();
}
void MainWindow::closedWebcamWindow(){
    webOBJ->stopFrameCapture();
    ui->BitPlane->setEnabled(true);
}
