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
    while (true) {
        cv::VideoCapture tempCap;
        if (tempCap.open(cameraCount)) {
            cameraList.append(QString("相机 %1").arg(cameraCount));
            cameraIndices.append(cameraCount);
            tempCap.release();
            cameraCount++;
        } else {
            break;
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
    
    // 打开指定相机
    if (cap.open(cameraIndex)) {
        currentCameraIndex = cameraIndex;
        return true;
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