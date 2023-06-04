#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <vector>
#include <opencv2/opencv.hpp>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void setupLabels();
    void onClicked();
    std::vector<cv::Mat> GenerateBit();

//    void generateBitPlanes();

private:
    Ui::MainWindow *ui;
    std::vector<cv::Mat> BitPlaneimages;
    std::vector<QLabel*> BitPlaneLables;
//    cv::Mat originalImage;
};
#endif // MAINWINDOW_H
