#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->main_pic->setScaledContents(true);
    onBitplane=0;

    connect(ui->BitPlane, &QPushButton::clicked, this, &MainWindow::onClicked);
    connect(ui->LIveBut, &QPushButton::clicked, this , &MainWindow::onClicked);

    webcamActivated = false;
    thresholdActivated = false;
    ui->Bitcheckbox->setEnabled(false);
    ui->threscheckbox->setEnabled(false);
    ui->thres_invcheckbox->setEnabled(false);

    BitPosition = 7;
    ui->slidebar->setValue(BitPosition);

    ui->webcamlab->setScaledContents(true);

    connect(ui->slidebar,&QSlider::valueChanged,this,&MainWindow::setBitPosition);

    connect(ui->Bitcheckbox,&QCheckBox::clicked,this,&MainWindow::handleCheckboxClicked);
    connect(ui->threscheckbox,&QCheckBox::clicked,this,&MainWindow::handleCheckboxClicked);
    connect(ui->thres_invcheckbox,&QCheckBox::clicked,this,&MainWindow::handleCheckboxClicked);

    //next
    connect(ui->nextpic,&QPushButton::clicked,this,&MainWindow::changePicture);
    connect(ui->lastpic,&QPushButton::clicked,this,&MainWindow::changePicture);

    connect(&timer, &QTimer::timeout, this, &MainWindow::updateFrames);

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
        ui->lastpic->setEnabled(true);
        ui->nextpic->setEnabled(true);

        QImage original_image(BitPlaneimages[8].data, BitPlaneimages[8].cols, BitPlaneimages[8].rows, BitPlaneimages[8].step, QImage::Format_Grayscale8);
        ui->main_pic->setPixmap(QPixmap::fromImage(original_image));
        }
    }
else if (ChosenBut==ui->LIveBut){

    if (webcamActivated){
        webcamActivated = false;
        ui->BitPlane->setEnabled(true);
        ui->LIveBut->setText("Start Live Mode");
        stopFrameCapture();
        ui->Bitcheckbox->setChecked(false);
        ui->threscheckbox->setChecked(false);
        ui->thres_invcheckbox->setChecked(false);
        ui->Bitcheckbox->setEnabled(false);
        ui->threscheckbox->setEnabled(false);
        ui->thres_invcheckbox->setEnabled(false);
    }
    else{
        webcamActivated = true;
        ui->BitPlane->setEnabled(false);

        ui->main_picelabel->clear();
        ui->main_pic->clear();

        ui->LIveBut->setText("Stop Live Mode");
        ui->Bitcheckbox->setEnabled(true);
        ui->threscheckbox->setEnabled(true);
        ui->thres_invcheckbox->setEnabled(true);
        }
    }
else
    qDebug() <<"not clicekd";

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

    cv::Mat slicedImage(image.rows, image.cols, CV_8UC1);

    for (int plane = 0; plane < 8; ++plane)
        BitPlaneimages.push_back(GenerateBitSlice(image,plane));

    BitPlaneimages.push_back(image);
    return BitPlaneimages;
}

void MainWindow::startFrameCapture()
{
    capture.open(0);
    if (!capture.isOpened())
        {
            std::cout<<"can not open webcam bro";
            return;
        }

    timer.start(33); // 30 FPS
}

void MainWindow::stopFrameCapture()
{
    timer.stop();
    capture.release();
    ui->webcamlab->clear();
}

cv::Mat MainWindow::GenerateBitSlice(cv::Mat frame,int Bitpos){

    if (frame.empty())
    {
        return cv::Mat();
    }

    cv::Mat slicedImage = cv::Mat::zeros(frame.size(), CV_8UC1);

       for (int i = 0; i < frame.rows; i++)
       {
           for (int j = 0; j < frame.cols; j++)
           {
               slicedImage.at<uchar>(i, j) = (frame.at<uchar>(i, j) >> Bitpos) & 1;
               slicedImage.at<uchar>(i, j) *= 255;
           }
       }

    return slicedImage;
}

void MainWindow::updateFrames(){

    if (bitsliceActivated)
    {
        cv::Mat frame;
        capture >> frame;

        if (frame.empty())
        {
            return;
        }

        cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);

        cv::Mat slicedImage = GenerateBitSlice(frame,BitPosition);

        QImage qimage(slicedImage.data, slicedImage.cols, slicedImage.rows, slicedImage.step, QImage::Format_Grayscale8);

        ui->webcamlab->setPixmap(QPixmap::fromImage(qimage));
        ui->webcamlab->setScaledContents(true);
    }

    else if (thresholdActivated || thres_invActivated)
    {
        cv::Mat frame;
        capture >> frame;

        if (frame.empty())
        {
            return;
        }

        cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);

        cv::Mat slicedImage = cv::Mat::zeros(frame.size(), CV_8UC1);

        cv::threshold(frame,slicedImage,Threshold,255,th_binary);

        QImage qimage(slicedImage.data, slicedImage.cols, slicedImage.rows, slicedImage.step, QImage::Format_Grayscale8);

        ui->webcamlab->setPixmap(QPixmap::fromImage(qimage));
//        ui->webcamlab->setScaledContents(true);
    }
}

void MainWindow::handleCheckboxClicked()
{
    if (ui->Bitcheckbox->isChecked()) {
        bitsliceActivated = true;
        thresholdActivated = false;
        thres_invActivated = false;

        ui->threscheckbox->setEnabled(false);
        ui->thres_invcheckbox->setEnabled(false);

        ui->slidebar->setRange(0,7);
        startFrameCapture();

    }
    else if (ui->threscheckbox->isChecked()) {
        bitsliceActivated = false;
        thresholdActivated = true;
        thres_invActivated = false;

        ui->Bitcheckbox->setEnabled(false);
        ui->thres_invcheckbox->setEnabled(false);

        ui->slidebar->setRange(0,255);
        th_binary=0;
        startFrameCapture();


    } else if (ui->thres_invcheckbox->isChecked()) {
        bitsliceActivated = false;
        thresholdActivated = false;
        thres_invActivated = true;

        ui->threscheckbox->setEnabled(false);
        ui->Bitcheckbox->setEnabled(false);

        ui->slidebar->setRange(0,255);
        th_binary=1;
        startFrameCapture();


    } else {
        stopFrameCapture();
        bitsliceActivated = false;
        thresholdActivated = false;
        thres_invActivated = false;

        ui->Bitcheckbox->setEnabled(true);
        ui->threscheckbox->setEnabled(true);
        ui->thres_invcheckbox->setEnabled(true);
    }
}

void MainWindow::setBitPosition(int value){

    if (bitsliceActivated){
        BitPosition = value;
        ui->bitlabel->setText("Bit Position : "+QString::number(BitPosition));
    }

    else if (thresholdActivated){
        Threshold = value;
        ui->bitlabel->setText("Threshold : "+QString::number(Threshold));
    }
    else if (thres_invActivated){
        Threshold = value;
        ui->bitlabel->setText("Threshold inv: "+QString::number(Threshold));
    }
}

void MainWindow::changePicture(){
    QObject *next_last = QObject::sender();

    if (next_last == ui->nextpic){
        onBitplane++;
        if (onBitplane>7){
            onBitplane--;
            return;
        }
    }
    else{
        onBitplane--;
        if (onBitplane<0){
            onBitplane++;
            return;
        }
    }

    QImage selectedPlane(BitPlaneimages[onBitplane].data, BitPlaneimages[onBitplane].cols, BitPlaneimages[onBitplane].rows, BitPlaneimages[onBitplane].step, QImage::Format_Grayscale8);
    ui->bitplanelapic->setPixmap(QPixmap::fromImage(selectedPlane));
    ui->bitplanelabel->setText("bitplane number " + QString::number(onBitplane));

}
