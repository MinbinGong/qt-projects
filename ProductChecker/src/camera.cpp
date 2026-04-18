#include "camera.h"

Camera::Camera(QObject *parent) : QObject(parent), currentCameraIndex(-1)
{
}

Camera::~Camera()
{
    close();
}

void Camera::detectCameras()
{
    cameraList.clear();
    cameraIndices.clear();
    
    int cameraCount = 0;
    // 尝试检测前10个相机索引
    for (int i = 0; i < 10; i++) {
        cv::VideoCapture tempCap;
        if (tempCap.open(i)) {
            cameraList.append(QString("相机 %1").arg(i));
            cameraIndices.append(i);
            tempCap.release();
            cameraCount++;
        }
    }
}

QStringList Camera::getCameraList() const
{
    return cameraList;
}

bool Camera::open(int cameraIndex)
{
    // 关闭当前相机
    close();
    
    // 尝试打开指定相机
    if (cap.open(cameraIndex)) {
        currentCameraIndex = cameraIndex;
        return true;
    }
    
    // 尝试使用不同的API打开相机
    for (int api = cv::CAP_DSHOW; api <= cv::CAP_MSMF; api++) {
        if (cap.open(cameraIndex, api)) {
            currentCameraIndex = cameraIndex;
            return true;
        }
    }
    
    return false;
}

void Camera::close()
{
    if (cap.isOpened()) {
        cap.release();
        currentCameraIndex = -1;
    }
}

bool Camera::isOpened() const
{
    return cap.isOpened();
}

cv::Mat Camera::captureFrame()
{
    cv::Mat frame;
    if (cap.isOpened()) {
        cap >> frame;
    }
    return frame;
}

int Camera::getCurrentCameraIndex() const
{
    return currentCameraIndex;
}