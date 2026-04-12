#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <opencv2/opencv.hpp>
#include "camera.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void loadBaseImage();
    void compareImages();
    void toggleCamera();
    void onCameraChanged(int index);

private:
    Ui::MainWindow *ui;
    QImage baseImage;
    QImage cameraImage;
    cv::Mat baseMat;
    cv::Mat cameraMat;
    Camera *camera;
    QTimer *timer;
    bool cameraActive;
};

#endif // MAINWINDOW_H
