#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>

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
    : QMainWindow(parent), ui(new Ui::MainWindow), cameraActive(false)
{
    ui->setupUi(this);

    // 创建相机对象
    camera = new Camera(this);

    // 检测可用的相机
    camera->detectCameras();
    
    // 填充相机下拉列表
    ui->cameraComboBox->clear();
    QStringList cameraList = camera->getCameraList();
    for (int i = 0; i < cameraList.size(); i++) {
        ui->cameraComboBox->addItem(cameraList[i], i);
    }

    // 确定要使用的相机索引
    int cameraIndex = -1;
    int cameraCount = cameraList.size();
    
    if (cameraCount == 0) {
        ui->resultLabel->setText("未检测到可用相机");
        ui->resultLabel->setStyleSheet("color: red;");
    } else if (cameraCount == 1) {
        // 只有一个相机，直接使用
        cameraIndex = 0;
    } else {
        // 多个相机，让用户选择
        bool ok;
        QString selectedCamera = QInputDialog::getItem(
            this, "选择相机", "请选择要使用的相机:", cameraList, 0, false, &ok);
        
        if (ok) {
            cameraIndex = cameraList.indexOf(selectedCamera);
            if (cameraIndex >= 0) {
                ui->cameraComboBox->setCurrentIndex(cameraIndex);
            }
        } else {
            // 用户取消选择，使用第一个相机
            cameraIndex = 0;
        }
    }

    // 打开选择的相机
    if (cameraIndex >= 0 && cameraIndex < cameraCount) {
        bool success = camera->open(cameraIndex);
        if (success) {
            ui->resultLabel->setText("相机已就绪，请点击拍摄图片");
            ui->resultLabel->setStyleSheet("color: blue;");
            cameraActive = true;
        } else {
            ui->resultLabel->setText("无法打开相机");
            ui->resultLabel->setStyleSheet("color: red;");
        }
    }

    connect(ui->loadBaseBtn, &QPushButton::clicked, this, &MainWindow::loadBaseImage);
    connect(ui->compareBtn, &QPushButton::clicked, this, &MainWindow::compareImages);
    connect(ui->cameraBtn, &QPushButton::clicked, this, &MainWindow::toggleCamera);
    connect(ui->cameraComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onCameraChanged);
}

MainWindow::~MainWindow()
{
    delete camera;
    delete ui;
}

void MainWindow::loadBaseImage()
{
    QString fileName = QFileDialog::getOpenFileName(this, "选择基准图片", "", "图片文件 (*.png *.jpg *.jpeg)");
    if (!fileName.isEmpty()) {
        ui->resultLabel->setText(QString("正在加载图片: %1").arg(fileName));
        ui->resultLabel->setStyleSheet("color: blue;");
        
        // 检查文件是否存在
        QFileInfo fileInfo(fileName);
        if (!fileInfo.exists()) {
            ui->resultLabel->setText("错误: 文件不存在");
            ui->resultLabel->setStyleSheet("color: red;");
            return;
        }
        
        // 尝试加载图片
        baseMat = cv::imread(fileName.toStdString());
        if (!baseMat.empty()) {
            baseImage = matToQImage(baseMat);
            QPixmap pixmap = QPixmap::fromImage(baseImage.scaled(ui->baseImageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
            ui->baseImageLabel->setPixmap(pixmap);
            ui->resultLabel->setText("基准图片加载成功");
            ui->resultLabel->setStyleSheet("color: green;");
        } else {
            ui->resultLabel->setText("错误: 图片加载失败");
            ui->resultLabel->setStyleSheet("color: red;");
        }
    }
}

void MainWindow::compareImages()
{
    if (baseMat.empty() || cameraMat.empty()) {
        ui->resultLabel->setText("请先加载基准图片并拍摄图片");
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

void MainWindow::onCameraChanged(int index)
{
    // 打开选择的相机
    bool success = camera->open(index);
    if (success) {
        ui->resultLabel->setText("相机已就绪，请点击拍摄图片");
        ui->resultLabel->setStyleSheet("color: blue;");
        cameraActive = true;
    } else {
        ui->resultLabel->setText("无法打开选中的相机");
        ui->resultLabel->setStyleSheet("color: red;");
        cameraActive = false;
    }
}

void MainWindow::toggleCamera()
{
    if (camera->isOpened()) {
        // 捕获一张照片
        cameraMat = camera->captureFrame();
        if (!cameraMat.empty()) {
            cameraImage = matToQImage(cameraMat);
            // 计算窗口大小的45%
            int maxWidth = this->width() * 0.45;
            int maxHeight = this->height() * 0.45;
            QSize maxSize(maxWidth, maxHeight);
            // 缩放到窗口大小的45%，保持宽高比
            QPixmap pixmap = QPixmap::fromImage(cameraImage.scaled(maxSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            ui->cameraImageLabel->setPixmap(pixmap);
            ui->resultLabel->setText("已捕获图片，可进行比较");
            ui->resultLabel->setStyleSheet("color: blue;");
        }
    } else {
        // 如果相机未打开，尝试重新打开
        int cameraIndex = ui->cameraComboBox->currentIndex();
        bool success = camera->open(cameraIndex);
        if (success) {
            // 捕获一张照片
            cameraMat = camera->captureFrame();
            if (!cameraMat.empty()) {
                cameraImage = matToQImage(cameraMat);
                // 计算窗口大小的45%
                int maxWidth = this->width() * 0.45;
                int maxHeight = this->height() * 0.45;
                QSize maxSize(maxWidth, maxHeight);
                // 缩放到窗口大小的45%，保持宽高比
                QPixmap pixmap = QPixmap::fromImage(cameraImage.scaled(maxSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                ui->cameraImageLabel->setPixmap(pixmap);
                ui->resultLabel->setText("已捕获图片，可进行比较");
                ui->resultLabel->setStyleSheet("color: blue;");
            }
        } else {
            ui->resultLabel->setText("无法打开相机");
            ui->resultLabel->setStyleSheet("color: red;");
        }
    }
}
