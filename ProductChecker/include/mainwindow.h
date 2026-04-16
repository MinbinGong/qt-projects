#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "camera.h"
#include <opencv2/opencv.hpp>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void captureBaseImage();
    void compareImages();
    void toggleCamera();
    void onCameraChanged(int index);
    void updateVideoStream();

private:
    Ui::MainWindow *ui;
    Camera *camera;
    QTimer *videoTimer;
    bool cameraActive;
    bool videoStreamActive;
    cv::Mat baseMat;
    cv::Mat cameraMat;
    cv::Mat previousFrame;
    cv::Mat motionMask;
    QImage baseImage;
    QImage cameraImage;
};

#endif // MAINWINDOW_H
