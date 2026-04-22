#include "detectionresultwindow.h"
#include <QMessageBox>
#include <QImage>
#include <QHBoxLayout>

DetectionResultWindow::DetectionResultWindow(const QString &title, QWidget *parent)
    : QWidget(parent), m_currentIndex(0)
{
    setWindowTitle(title);
    resize(800, 600);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    m_imageLabel = new QLabel(this);
    m_imageLabel->setAlignment(Qt::AlignCenter);
    m_imageLabel->setStyleSheet("border: 1px solid gray;");
    mainLayout->addWidget(m_imageLabel);

    m_objectInfoLabel = new QLabel("未检测到物品", this);
    m_objectInfoLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(m_objectInfoLabel);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    m_previousBtn = new QPushButton("上一个", this);
    m_nextBtn = new QPushButton("下一个", this);
    buttonLayout->addWidget(m_previousBtn);
    buttonLayout->addWidget(m_nextBtn);

    m_objectCountLabel = new QLabel("0 / 0", this);
    buttonLayout->addWidget(m_objectCountLabel);

    mainLayout->addLayout(buttonLayout);

    connect(m_previousBtn, &QPushButton::clicked, this, &DetectionResultWindow::onPreviousClicked);
    connect(m_nextBtn, &QPushButton::clicked, this, &DetectionResultWindow::onNextClicked);

    updateDisplay();
}

void DetectionResultWindow::setImage(const cv::Mat &image)
{
    m_originalImage = image.clone();
}

void DetectionResultWindow::setDetectedObjects(const std::vector<DetectedObject> &objects)
{
    m_objects = objects;
    m_currentIndex = 0;
    updateDisplay();
}

void DetectionResultWindow::updateDisplay()
{
    if (m_objects.empty()) {
        m_imageLabel->setText("没有检测到物品");
        m_objectInfoLabel->setText("未检测到物品");
        m_objectCountLabel->setText("0 / 0");
        return;
    }

    // 只显示检测到的物品，而不是完整的基准图片
    cv::Mat displayImage = m_objects[m_currentIndex].image.clone();

    QImage qimage = QImage(displayImage.data, displayImage.cols, displayImage.rows, displayImage.step, QImage::Format_RGB888).rgbSwapped();
    QPixmap pixmap = QPixmap::fromImage(qimage);
    m_imageLabel->setPixmap(pixmap.scaled(m_imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

    m_objectInfoLabel->setText(QString("物品 %1: %2 (%.1f%%) 位置(%3, %4) 大小(%5x%6)")
                                    .arg(m_objects[m_currentIndex].id)
                                    .arg(QString::fromStdString(m_objects[m_currentIndex].className))
                                    .arg(m_objects[m_currentIndex].confidence * 100)
                                    .arg(m_objects[m_currentIndex].boundingBox.x)
                                    .arg(m_objects[m_currentIndex].boundingBox.y)
                                    .arg(m_objects[m_currentIndex].boundingBox.width)
                                    .arg(m_objects[m_currentIndex].boundingBox.height));

    m_objectCountLabel->setText(QString("%1 / %2").arg(m_currentIndex + 1).arg(m_objects.size()));

    m_previousBtn->setEnabled(m_currentIndex > 0);
    m_nextBtn->setEnabled(m_currentIndex < (int)m_objects.size() - 1);
}

void DetectionResultWindow::onPreviousClicked()
{
    if (m_currentIndex > 0) {
        m_currentIndex--;
        updateDisplay();
    }
}

void DetectionResultWindow::onNextClicked()
{
    if (m_currentIndex < (int)m_objects.size() - 1) {
        m_currentIndex++;
        updateDisplay();
    }
}

void DetectionResultWindow::onObjectClicked(int objectId)
{
    for (size_t i = 0; i < m_objects.size(); i++) {
        if (m_objects[i].id == objectId) {
            m_currentIndex = i;
            updateDisplay();
            break;
        }
    }
}
