#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QDialog>
#include <QLabel>
#include "camera.h"
#include <opencv2/opencv.hpp>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

struct DetectedObject {
    cv::Rect boundingBox;
    cv::Mat image;
    int id;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void captureBaseImage();
    void toggleCamera();
    void updateVideoStream();
    void resetCaptureCooldown();
    void detectProducts();
    void compareDetectedObjects();

private:
    std::vector<DetectedObject> detectObjectsInImage(const cv::Mat &image);
    QImage matToQImage(const cv::Mat &mat);
    void displayDetectionResult(const cv::Mat &image, const std::vector<DetectedObject> &objects, QLabel *label);

    Ui::MainWindow *ui;
    Camera *camera;
    QTimer *videoTimer;
    QTimer *motionCaptureTimer;
    bool cameraActive;
    bool videoStreamActive;
    bool motionDetected;
    bool captureEnabled;
    int captureCooldown;
    cv::Mat baseMat;
    cv::Mat cameraMat;
    cv::Mat previousFrame;
    cv::Mat motionMask;
    QImage baseImage;
    QImage cameraImage;
    std::vector<DetectedObject> baseDetectedObjects;
    std::vector<DetectedObject> captureDetectedObjects;
    QDialog *compareResultDialog;
};

#endif // MAINWINDOW_H
