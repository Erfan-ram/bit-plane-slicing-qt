#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupLabels();

    connect(ui->BitPlane, &QPushButton::clicked, this, &MainWindow::onClicked);
    connect(ui->LIveBut, &QPushButton::clicked, this , &MainWindow::onClicked);

    webcamActivated = false;
    thresholdActivated = false;
    ui->Bitcheckbox->setEnabled(false);
    ui->threscheckbox->setEnabled(false);
    ui->thres_invcheckbox->setEnabled(false);

    BitPosition = 7;
    ui->bitScrollBar->setValue(BitPosition);

    connect(ui->bitScrollBar,&QScrollBar::valueChanged,this,&MainWindow::setBitPosition);

    connect(ui->Bitcheckbox,&QCheckBox::clicked,this,&MainWindow::handleCheckboxClicked);
    connect(ui->threscheckbox,&QCheckBox::clicked,this,&MainWindow::handleCheckboxClicked);
    connect(ui->thres_invcheckbox,&QCheckBox::clicked,this,&MainWindow::handleCheckboxClicked);

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
        ui->LIveBut->setText("Stop Live Mode");
        ui->Bitcheckbox->setEnabled(true);
        ui->threscheckbox->setEnabled(true);
        ui->thres_invcheckbox->setEnabled(true);
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

    cv::Mat slicedImage(image.rows, image.cols, CV_8UC1);

    for (int plane = 0; plane < 8; ++plane)
        BitPlaneimages.push_back(GenerateBitSlice(image,plane));

    return BitPlaneimages;
}

void MainWindow::startFrameCapture(int mode)
{
    capture.open(0);
    if (!capture.isOpened())
        {
            std::cout<<"can not open webcam bro";
            return;
        }
    if (mode==1)
        connect(&timer, &QTimer::timeout, this, &MainWindow::updateBitSliceFrame);

    else
        connect(&timer, &QTimer::timeout, this, &MainWindow::updateThresholdFrame);

    timer.start(33); // 30 FPS
}

void MainWindow::stopFrameCapture()
{
    timer.stop();
    capture.release();
}

void MainWindow::updateBitSliceFrame()
{
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

void MainWindow::updateThresholdFrame(){

    if (thresholdActivated || thres_invActivated)
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
        ui->webcamlab->setScaledContents(true);
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

        ui->bitScrollBar->setRange(0,7);
        startFrameCapture(1);

    }
    else if (ui->threscheckbox->isChecked()) {
        bitsliceActivated = false;
        thresholdActivated = true;
        thres_invActivated = false;

        ui->Bitcheckbox->setEnabled(false);
        ui->thres_invcheckbox->setEnabled(false);

        ui->bitScrollBar->setRange(0,255);
        th_binary=0;
        startFrameCapture(2);


    } else if (ui->thres_invcheckbox->isChecked()) {
        bitsliceActivated = false;
        thresholdActivated = false;
        thres_invActivated = true;

        ui->threscheckbox->setEnabled(false);
        ui->Bitcheckbox->setEnabled(false);

        ui->bitScrollBar->setRange(0,255);
        th_binary=1;
        startFrameCapture(2);


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
        ui->threshlabel->setText("Threshold : "+QString::number(Threshold));
    }
    else if (thres_invActivated){
        Threshold = value;
        ui->threshlabel->setText("Threshold : "+QString::number(Threshold));
    }
}
