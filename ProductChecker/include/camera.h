#ifndef CAMERA_H
#define CAMERA_H

#include <QObject>
#include <opencv2/opencv.hpp>

class Camera : public QObject
{
    Q_OBJECT

public:
    explicit Camera(QObject *parent = nullptr);
    ~Camera();

    // 检测可用的相机
    void detectCameras();
    
    // 获取相机列表
    QStringList getCameraList() const;
    
    // 打开指定索引的相机
    bool open(int cameraIndex);
    
    // 关闭相机
    void close();
    
    // 检查相机是否打开
    bool isOpened() const;
    
    // 捕获一帧图像
    cv::Mat captureFrame();
    
    // 获取当前相机索引
    int getCurrentCameraIndex() const;

private:
    cv::VideoCapture cap;
    QStringList cameraList;
    QList<int> cameraIndices;
    int currentCameraIndex;
};

#endif // CAMERA_H