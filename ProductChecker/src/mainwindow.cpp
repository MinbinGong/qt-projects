#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QImage>

// 辅助函数：将OpenCV的Mat转换为QImage
QImage matToQImage(const cv::Mat &mat)
{
    if (mat.empty()) {
        return QImage();
    }

    // 处理不同的图像格式
    if (mat.type() == CV_8UC3) {
        // BGR格式转换为RGB
        cv::Mat rgbMat;
        cv::cvtColor(mat, rgbMat, cv::COLOR_BGR2RGB);
        return QImage(rgbMat.data, rgbMat.cols, rgbMat.rows, rgbMat.step, QImage::Format_RGB888).copy();
    } else if (mat.type() == CV_8UC1) {
        // 灰度图像
        return QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8).copy();
    }

    return QImage();
}



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), cameraActive(false), videoStreamActive(false), motionDetected(false), captureEnabled(false), captureCooldown(0)
{
    ui->setupUi(this);

    // 创建相机对象
    camera = new Camera(this);

    // 创建视频定时器
    videoTimer = new QTimer(this);
    connect(videoTimer, &QTimer::timeout, this, &MainWindow::updateVideoStream);
    
    // 创建运动捕获定时器
    motionCaptureTimer = new QTimer(this);
    motionCaptureTimer->setInterval(1000); // 1秒
    connect(motionCaptureTimer, &QTimer::timeout, this, &MainWindow::resetCaptureCooldown);

    // 检测可用的相机
    camera->detectCameras();
    
    // 获取相机列表
    QStringList cameraList = camera->getCameraList();
    int cameraCount = cameraList.size();
    int selectedCameraIndex = -1;
    
    if (cameraCount == 0) {
        ui->resultLabel->setText("未检测到可用相机");
        ui->resultLabel->setStyleSheet("color: red;");
    } else if (cameraCount == 1) {
        // 只有一个相机，直接使用
        selectedCameraIndex = 0;
    } else {
        // 多个相机，让用户选择
        bool ok;
        QString selectedCamera = QInputDialog::getItem(
            this, "选择相机", "请选择要使用的相机:", cameraList, 0, false, &ok);
        
        if (ok) {
            selectedCameraIndex = cameraList.indexOf(selectedCamera);
        } else {
            // 用户取消选择，使用第一个相机
            selectedCameraIndex = 0;
        }
    }
    
    // 打开选择的相机
    if (selectedCameraIndex >= 0 && selectedCameraIndex < cameraCount) {
        // 获取实际的相机索引
        int actualCameraIndex = selectedCameraIndex;
        bool success = camera->open(actualCameraIndex);
        if (success) {
            ui->resultLabel->setText("相机已就绪，点击开始视频流");
            ui->resultLabel->setStyleSheet("color: blue;");
            cameraActive = true;
        } else {
            ui->resultLabel->setText("无法打开相机");
            ui->resultLabel->setStyleSheet("color: red;");
        }
    }

    connect(ui->captureBaseBtn, &QPushButton::clicked, this, &MainWindow::captureBaseImage);
    connect(ui->captureImageBtn, &QPushButton::clicked, this, &MainWindow::captureImageAndCompare);
    connect(ui->compareBtn, &QPushButton::clicked, this, &MainWindow::compareImages);
    connect(ui->cameraBtn, &QPushButton::clicked, this, &MainWindow::toggleCamera);
}

MainWindow::~MainWindow()
{
    delete camera;
    delete videoTimer;
    delete motionCaptureTimer;
    delete ui;
}

void MainWindow::compareImages()
{
    if (baseMat.empty() || cameraMat.empty()) {
        ui->resultLabel->setText("请先拍摄基准图片并拍摄图片");
        return;
    }

    // 调整图片大小，使它们具有相同的尺寸
    cv::Mat resizedBase;
    cv::resize(baseMat, resizedBase, cameraMat.size());

    // 转换为灰度图进行比较
    cv::Mat grayBase, grayCamera;
    cv::cvtColor(resizedBase, grayBase, cv::COLOR_BGR2GRAY);
    cv::cvtColor(cameraMat, grayCamera, cv::COLOR_BGR2GRAY);

    // 计算绝对差异
    cv::Mat diff;
    cv::absdiff(grayBase, grayCamera, diff);

    // 应用阈值来突出差异
    cv::Mat thresh;
    cv::threshold(diff, thresh, 30, 255, cv::THRESH_BINARY);

    // 计算差异像素数
    int diffCount = cv::countNonZero(thresh);
    double totalPixels = resizedBase.total();
    double diffPercentage = (diffCount / totalPixels) * 100;

    // 计算平均差异值
    cv::Scalar meanDiff = cv::mean(diff);
    double avgDiff = meanDiff[0];

    if (diffPercentage < 5 && avgDiff < 30) {
        ui->resultLabel->setText(QString("图片相似，差异率: %1%, 平均差异: %2").arg(diffPercentage, 0, 'f', 2).arg(avgDiff, 0, 'f', 2));
        ui->resultLabel->setStyleSheet("color: green;");
    } else {
        ui->resultLabel->setText(QString("图片差异较大，差异率: %1%, 平均差异: %2").arg(diffPercentage, 0, 'f', 2).arg(avgDiff, 0, 'f', 2));
        ui->resultLabel->setStyleSheet("color: red;");
    }
}



void MainWindow::toggleCamera()
{
    if (camera->isOpened()) {
        if (videoStreamActive) {
            // 停止视频流
            videoTimer->stop();
            motionCaptureTimer->stop();
            videoStreamActive = false;
            captureEnabled = false;
            ui->cameraBtn->setText("开始视频流");
            ui->resultLabel->setText("视频流已停止，可进行比较");
            ui->resultLabel->setStyleSheet("color: blue;");
        } else {
            // 开始视频流
            videoTimer->start(33); // 约30fps
            videoStreamActive = true;
            captureEnabled = true;
            captureCooldown = 0;
            ui->cameraBtn->setText("停止视频流");
            ui->resultLabel->setText("视频流已开始，运动检测自动抓取已启用");
            ui->resultLabel->setStyleSheet("color: green;");
        }
    } else {
        // 如果相机未打开，尝试重新打开默认相机
        bool success = camera->open(0);
        if (success) {
            // 开始视频流
            videoTimer->start(33); // 约30fps
            videoStreamActive = true;
            captureEnabled = true;
            captureCooldown = 0;
            ui->cameraBtn->setText("停止视频流");
            ui->resultLabel->setText("视频流已开始，运动检测自动抓取已启用");
            ui->resultLabel->setStyleSheet("color: green;");
        } else {
            ui->resultLabel->setText("无法打开相机");
            ui->resultLabel->setStyleSheet("color: red;");
        }
    }
}

void MainWindow::captureBaseImage()
{
    bool cameraWasOpen = camera->isOpened();
    bool videoStreamWasActive = videoStreamActive;
    
    if (cameraWasOpen) {
        // 如果视频流正在运行，暂停视频流，捕获一帧图像
        if (videoStreamWasActive) {
            videoTimer->stop();
            baseMat = camera->captureFrame();
            videoTimer->start(33); // 恢复视频流
        } else {
            // 直接捕获一帧图像
            baseMat = camera->captureFrame();
        }
        
        if (!baseMat.empty()) {
            baseImage = matToQImage(baseMat);
            QPixmap pixmap = QPixmap::fromImage(baseImage.scaled(ui->baseImageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
            ui->baseImageLabel->setPixmap(pixmap);
            ui->resultLabel->setText("基准图片拍摄成功");
            ui->resultLabel->setStyleSheet("color: green;");
        } else {
            ui->resultLabel->setText("无法捕获基准图片");
            ui->resultLabel->setStyleSheet("color: red;");
        }
    } else {
        // 如果相机未打开，尝试打开默认相机
        bool success = camera->open(0);
        if (success) {
            // 捕获一帧图像作为基准图片
            baseMat = camera->captureFrame();
            if (!baseMat.empty()) {
                baseImage = matToQImage(baseMat);
                QPixmap pixmap = QPixmap::fromImage(baseImage.scaled(ui->baseImageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
                ui->baseImageLabel->setPixmap(pixmap);
                ui->resultLabel->setText("基准图片拍摄成功");
                ui->resultLabel->setStyleSheet("color: green;");
            } else {
                ui->resultLabel->setText("无法捕获基准图片");
                ui->resultLabel->setStyleSheet("color: red;");
            }
        } else {
            ui->resultLabel->setText("无法打开相机");
            ui->resultLabel->setStyleSheet("color: red;");
        }
    }
}

void MainWindow::captureImageAndCompare()
{
    if (camera->isOpened()) {
        // 从视频流中捕获一帧图像
        cameraMat = camera->captureFrame();
        if (!cameraMat.empty()) {
            // 更新相机图像显示
            cameraImage = matToQImage(cameraMat);
            int maxWidth = this->width() * 0.45;
            int maxHeight = this->height() * 0.45;
            QSize maxSize(maxWidth, maxHeight);
            QPixmap pixmap = QPixmap::fromImage(cameraImage.scaled(maxSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            ui->cameraImageLabel->setPixmap(pixmap);
            
            // 自动与基准图片进行比对
            compareImages();
        } else {
            ui->resultLabel->setText("无法从视频流中抓取图片");
            ui->resultLabel->setStyleSheet("color: red;");
        }
    } else {
        ui->resultLabel->setText("相机未打开");
        ui->resultLabel->setStyleSheet("color: red;");
    }
}

void MainWindow::resetCaptureCooldown()
{
    if (captureCooldown > 0) {
        captureCooldown--;
    }
}



void MainWindow::updateVideoStream()
{
    if (camera->isOpened()) {
        // 捕获视频帧
        cameraMat = camera->captureFrame();
        if (!cameraMat.empty()) {
            // 应用运动侦测
            // 转换为灰度图
            cv::Mat grayFrame;
            cv::cvtColor(cameraMat, grayFrame, cv::COLOR_BGR2GRAY);
            
            // 高斯模糊
            cv::GaussianBlur(grayFrame, grayFrame, cv::Size(21, 21), 0);
            
            // 如果是第一帧，保存为前一帧
            if (previousFrame.empty()) {
                grayFrame.copyTo(previousFrame);
            }
            
            // 计算帧差
            cv::absdiff(previousFrame, grayFrame, motionMask);
            
            // 应用阈值
            cv::threshold(motionMask, motionMask, 25, 255, cv::THRESH_BINARY);
            
            // 形态学操作
            cv::dilate(motionMask, motionMask, cv::Mat(), cv::Point(-1, -1), 2);
            cv::erode(motionMask, motionMask, cv::Mat(), cv::Point(-1, -1), 1);
            
            // 检测轮廓
            std::vector<std::vector<cv::Point>> contours;
            cv::findContours(motionMask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
            
            // 绘制轮廓
            motionDetected = false;
            for (size_t i = 0; i < contours.size(); i++) {
                // 过滤小轮廓
                if (cv::contourArea(contours[i]) < 500) {
                    continue;
                }
                
                // 绘制边界框
                cv::Rect boundingRect = cv::boundingRect(contours[i]);
                cv::rectangle(cameraMat, boundingRect, cv::Scalar(0, 255, 0), 2);
                
                // 检测到运动
                motionDetected = true;
            }
            
            // 保存当前帧为前一帧
            grayFrame.copyTo(previousFrame);
            
            // 根据运动检测自动抓取图片
            if (motionDetected && captureEnabled && captureCooldown == 0) {
                // 自动抓取图片并与基准图片进行比对
                captureImageAndCompare();
                
                // 设置冷却时间，避免频繁抓取
                captureCooldown = 5; // 5秒
                
                // 启动冷却时间定时器
                if (!motionCaptureTimer->isActive()) {
                    motionCaptureTimer->start();
                }
            }
            
            cameraImage = matToQImage(cameraMat);
            // 计算窗口大小的45%
            int maxWidth = this->width() * 0.45;
            int maxHeight = this->height() * 0.45;
            QSize maxSize(maxWidth, maxHeight);
            // 缩放到窗口大小的45%，保持宽高比
            QPixmap pixmap = QPixmap::fromImage(cameraImage.scaled(maxSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            ui->cameraImageLabel->setPixmap(pixmap);
        }
    }
}
