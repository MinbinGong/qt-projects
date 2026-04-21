#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QImage>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>

QImage MainWindow::matToQImage(const cv::Mat &mat)
{
    if (mat.empty()) {
        return QImage();
    }

    if (mat.type() == CV_8UC3) {
        cv::Mat rgbMat;
        cv::cvtColor(mat, rgbMat, cv::COLOR_BGR2RGB);
        return QImage(rgbMat.data, rgbMat.cols, rgbMat.rows, rgbMat.step, QImage::Format_RGB888).copy();
    } else if (mat.type() == CV_8UC1) {
        return QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8).copy();
    }

    return QImage();
}

std::vector<DetectedObject> MainWindow::detectObjectsInImage(const cv::Mat &image)
{
    std::vector<DetectedObject> detectedObjects;

    if (image.empty()) {
        return detectedObjects;
    }

    // 使用ObjectDetector类进行物品检测
    detectedObjects = objectDetector->detectObjects(image, 0.5);

    return detectedObjects;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), cameraActive(false), videoStreamActive(false), motionDetected(false), captureEnabled(false), captureCooldown(0),
      baseResultWindow(nullptr), compareResultDialog(nullptr)
{
    ui->setupUi(this);

    camera = new Camera(this);
    objectDetector = new ObjectDetector();

    // 加载YOLO模型
    std::string modelPath = "libs/YOLO3/yolov3.weights";
    std::string configPath = "libs/YOLO3/yolo3.cfg";
    std::string classesPath = "libs/YOLO3/coco.names";
    
    bool modelLoaded = objectDetector->loadModel(modelPath, configPath, classesPath);
    if (!modelLoaded) {
        ui->resultLabel->setText("无法加载检测模型，将使用基于轮廓的检测方法");
        ui->resultLabel->setStyleSheet("color: orange;");
    } else {
        ui->resultLabel->setText("模型加载成功");
        ui->resultLabel->setStyleSheet("color: green;");
    }

    videoTimer = new QTimer(this);
    connect(videoTimer, &QTimer::timeout, this, &MainWindow::updateVideoStream);

    motionCaptureTimer = new QTimer(this);
    motionCaptureTimer->setInterval(1000);
    connect(motionCaptureTimer, &QTimer::timeout, this, &MainWindow::resetCaptureCooldown);

    camera->detectCameras();

    QStringList cameraList = camera->getCameraList();
    int cameraCount = cameraList.size();
    int selectedCameraIndex = -1;

    if (cameraCount == 0) {
        ui->resultLabel->setText("未检测到可用相机");
        ui->resultLabel->setStyleSheet("color: red;");
    } else if (cameraCount == 1) {
        selectedCameraIndex = 0;
    } else {
        bool ok;
        QString selectedCamera = QInputDialog::getItem(
            this, "选择相机", "请选择要使用的相机:", cameraList, 0, false, &ok);

        if (ok) {
            selectedCameraIndex = cameraList.indexOf(selectedCamera);
        } else {
            selectedCameraIndex = 0;
        }
    }

    if (selectedCameraIndex >= 0 && selectedCameraIndex < cameraCount) {
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
    connect(ui->cameraBtn, &QPushButton::clicked, this, &MainWindow::toggleCamera);
    connect(ui->detectBtn, &QPushButton::clicked, this, &MainWindow::detectProducts);
    connect(ui->showBaseDetectBtn, &QPushButton::clicked, this, &MainWindow::showBaseDetectionResult);
    connect(ui->compareBtn, &QPushButton::clicked, this, &MainWindow::compareDetectedObjects);
}

MainWindow::~MainWindow()
{
    if (baseResultWindow) {
        delete baseResultWindow;
    }
    if (compareResultDialog) {
        delete compareResultDialog;
    }
    delete camera;
    delete objectDetector;
    delete videoTimer;
    delete motionCaptureTimer;
    delete ui;
}

void MainWindow::toggleCamera()
{
    if (camera->isOpened()) {
        if (videoStreamActive) {
            videoTimer->stop();
            motionCaptureTimer->stop();
            videoStreamActive = false;
            captureEnabled = false;
            ui->cameraBtn->setText("开始视频流");
            ui->resultLabel->setText("视频流已停止");
            ui->resultLabel->setStyleSheet("color: blue;");
        } else {
            videoTimer->start(33);
            videoStreamActive = true;
            captureEnabled = true;
            captureCooldown = 0;
            ui->cameraBtn->setText("停止视频流");
            ui->resultLabel->setText("视频流已开始");
            ui->resultLabel->setStyleSheet("color: green;");
        }
    } else {
        bool success = camera->open(0);
        if (success) {
            videoTimer->start(33);
            videoStreamActive = true;
            captureEnabled = true;
            captureCooldown = 0;
            ui->cameraBtn->setText("停止视频流");
            ui->resultLabel->setText("视频流已开始");
            ui->resultLabel->setStyleSheet("color: green;");
        } else {
            ui->resultLabel->setText("无法打开相机");
            ui->resultLabel->setStyleSheet("color: red;");
        }
    }
}

void MainWindow::displayDetectionResult(const cv::Mat &image, const std::vector<DetectedObject> &objects, QLabel *label)
{
    if (image.empty()) {
        return;
    }

    cv::Mat displayImage = image.clone();

    for (const auto &obj : objects) {
        cv::rectangle(displayImage, obj.boundingBox, cv::Scalar(0, 255, 0), 2);
        std::string labelText = obj.className + " (" + std::to_string(obj.confidence * 100) + "%)";
        cv::putText(displayImage, labelText, 
                    cv::Point(obj.boundingBox.x, obj.boundingBox.y - 10), 
                    cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 2);
    }

    QImage qimage = matToQImage(displayImage);
    QPixmap pixmap = QPixmap::fromImage(qimage.scaled(label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    label->setPixmap(pixmap);
}

void MainWindow::showBaseDetectionResult()
{
    if (!baseMat.empty()) {
        baseDetectedObjects = detectObjectsInImage(baseMat);

        if (!baseResultWindow) {
            baseResultWindow = new DetectionResultWindow("基准图片检测结果");
        }

        baseResultWindow->setImage(baseMat);
        baseResultWindow->setDetectedObjects(baseDetectedObjects);
        baseResultWindow->show();
    }
}

void MainWindow::captureBaseImage()
{
    bool cameraWasOpen = camera->isOpened();
    bool videoStreamWasActive = videoStreamActive;

    if (cameraWasOpen) {
        if (videoStreamWasActive) {
            videoTimer->stop();
            baseMat = camera->captureFrame();
            videoTimer->start(33);
        } else {
            baseMat = camera->captureFrame();
        }

        if (!baseMat.empty()) {
            baseImage = matToQImage(baseMat);
            
            // 自动检测基准图片中的物品并显示
            baseDetectedObjects = detectObjectsInImage(baseMat);
            displayDetectionResult(baseMat, baseDetectedObjects, ui->baseImageLabel);
            
            // 自动以独立窗口展示基准图片中的物品
            showBaseDetectionResult();
            
            ui->resultLabel->setText(QString("基准图片拍摄成功，检测到 %1 个物品").arg(baseDetectedObjects.size()));
            ui->resultLabel->setStyleSheet("color: green;");
        } else {
            ui->resultLabel->setText("无法捕获基准图片");
            ui->resultLabel->setStyleSheet("color: red;");
        }
    } else {
        bool success = camera->open(0);
        if (success) {
            baseMat = camera->captureFrame();
            if (!baseMat.empty()) {
                baseImage = matToQImage(baseMat);
                
                // 自动检测基准图片中的物品并显示
                baseDetectedObjects = detectObjectsInImage(baseMat);
                displayDetectionResult(baseMat, baseDetectedObjects, ui->baseImageLabel);
                
                // 自动以独立窗口展示基准图片中的物品
                showBaseDetectionResult();
                
                ui->resultLabel->setText(QString("基准图片拍摄成功，检测到 %1 个物品").arg(baseDetectedObjects.size()));
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

void MainWindow::resetCaptureCooldown()
{
    if (captureCooldown > 0) {
        captureCooldown--;
    }
}



void MainWindow::compareDetectedObjects()
{
    if (baseDetectedObjects.empty() || captureDetectedObjects.empty()) {
        ui->resultLabel->setText("请先检测基准图片和抓取图片中的物品");
        ui->resultLabel->setStyleSheet("color: red;");
        return;
    }

    if (compareResultDialog) {
        delete compareResultDialog;
    }

    compareResultDialog = new QDialog(this);
    compareResultDialog->setWindowTitle("物品对比结果");
    compareResultDialog->resize(600, 400);

    QVBoxLayout *mainLayout = new QVBoxLayout(compareResultDialog);

    QLabel *infoLabel = new QLabel(QString("基准图片检测到 %1 个物品，抓取图片检测到 %2 个物品")
                                       .arg(baseDetectedObjects.size())
                                       .arg(captureDetectedObjects.size()));
    infoLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(infoLabel);

    QScrollArea *scrollArea = new QScrollArea();
    QWidget *containerWidget = new QWidget();
    QVBoxLayout *itemsLayout = new QVBoxLayout(containerWidget);

    bool sameObjectFound = false;
    for (size_t i = 0; i < baseDetectedObjects.size(); i++) {
        for (size_t j = 0; j < captureDetectedObjects.size(); j++) {
            double baseArea = baseDetectedObjects[i].boundingBox.area();
            double captureArea = captureDetectedObjects[j].boundingBox.area();
            double areaRatio = baseArea / captureArea;

            if (areaRatio > 0.8 && areaRatio < 1.2) {
                sameObjectFound = true;
                QLabel *itemLabel = new QLabel(QString("物品 %1 与物品 %2 可能是同一个物品 (面积比率: %3%)")
                                                   .arg(i + 1)
                                                   .arg(j + 1)
                                                   .arg(areaRatio * 100, 0, 'f', 1));
                itemLabel->setStyleSheet("color: green; padding: 5px;");
                itemsLayout->addWidget(itemLabel);
            }
        }
    }

    if (!sameObjectFound) {
        QLabel *noMatchLabel = new QLabel("未检测到相同的物品");
        noMatchLabel->setAlignment(Qt::AlignCenter);
        noMatchLabel->setStyleSheet("color: red;");
        itemsLayout->addWidget(noMatchLabel);
    }

    scrollArea->setWidget(containerWidget);
    scrollArea->setWidgetResizable(true);
    mainLayout->addWidget(scrollArea);

    QPushButton *closeBtn = new QPushButton("关闭");
    mainLayout->addWidget(closeBtn);
    connect(closeBtn, &QPushButton::clicked, compareResultDialog, &QDialog::close);

    compareResultDialog->show();

    if (sameObjectFound) {
        ui->resultLabel->setText("检测到相同的物品");
        ui->resultLabel->setStyleSheet("color: green;");
    } else {
        ui->resultLabel->setText("未检测到相同的物品");
        ui->resultLabel->setStyleSheet("color: red;");
    }
}

void MainWindow::detectProducts()
{
    if (baseMat.empty()) {
        ui->resultLabel->setText("请先拍摄基准图片");
        ui->resultLabel->setStyleSheet("color: red;");
        return;
    }

    if (cameraMat.empty()) {
        ui->resultLabel->setText("请先抓取图片");
        ui->resultLabel->setStyleSheet("color: red;");
        return;
    }

    baseDetectedObjects = detectObjectsInImage(baseMat);
    captureDetectedObjects = detectObjectsInImage(cameraMat);

    if (baseDetectedObjects.empty()) {
        ui->resultLabel->setText("无法从基准图片中提取物品");
        ui->resultLabel->setStyleSheet("color: red;");
        return;
    }

    if (captureDetectedObjects.empty()) {
        ui->resultLabel->setText("无法从抓取图片中提取物品");
        ui->resultLabel->setStyleSheet("color: red;");
        return;
    }

    ui->resultLabel->setText(QString("基准图片: %1 个物品, 抓取图片: %2 个物品")
                                 .arg(baseDetectedObjects.size())
                                 .arg(captureDetectedObjects.size()));
    ui->resultLabel->setStyleSheet("color: blue;");
}

void MainWindow::updateVideoStream()
{
    if (camera->isOpened()) {
        cameraMat = camera->captureFrame();
        if (!cameraMat.empty()) {
            cv::Mat grayFrame;
            cv::cvtColor(cameraMat, grayFrame, cv::COLOR_BGR2GRAY);

            cv::GaussianBlur(grayFrame, grayFrame, cv::Size(21, 21), 0);

            if (previousFrame.empty()) {
                grayFrame.copyTo(previousFrame);
            }

            cv::absdiff(previousFrame, grayFrame, motionMask);

            cv::threshold(motionMask, motionMask, 25, 255, cv::THRESH_BINARY);

            cv::dilate(motionMask, motionMask, cv::Mat(), cv::Point(-1, -1), 2);
            cv::erode(motionMask, motionMask, cv::Mat(), cv::Point(-1, -1), 1);

            std::vector<std::vector<cv::Point>> contours;
            cv::findContours(motionMask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

            motionDetected = false;
            for (size_t i = 0; i < contours.size(); i++) {
                if (cv::contourArea(contours[i]) < 500) {
                    continue;
                }

                cv::Rect boundingRect = cv::boundingRect(contours[i]);
                cv::rectangle(cameraMat, boundingRect, cv::Scalar(0, 255, 0), 2);

                motionDetected = true;
            }

            grayFrame.copyTo(previousFrame);

            if (motionDetected && captureEnabled && captureCooldown == 0) {
                if (!cameraMat.empty()) {
                    // 自动检测抓取图片中的物品并显示
                    captureDetectedObjects = detectObjectsInImage(cameraMat);
                    displayDetectionResult(cameraMat, captureDetectedObjects, ui->cameraImageLabel);

                    ui->resultLabel->setText(QString("检测到运动，抓取图片并检测到 %1 个物品").arg(captureDetectedObjects.size()));
                    ui->resultLabel->setStyleSheet("color: green;");

                    // 自动对比检测结果
                    if (!baseDetectedObjects.empty() && !captureDetectedObjects.empty()) {
                        compareDetectedObjects();
                    }
                }

                captureCooldown = 5;

                if (!motionCaptureTimer->isActive()) {
                    motionCaptureTimer->start();
                }
            }

            cameraImage = matToQImage(cameraMat);
            int maxWidth = this->width() * 0.45;
            int maxHeight = this->height() * 0.45;
            QSize maxSize(maxWidth, maxHeight);
            QPixmap pixmap = QPixmap::fromImage(cameraImage.scaled(maxSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            ui->cameraImageLabel->setPixmap(pixmap);
        }
    }
}
