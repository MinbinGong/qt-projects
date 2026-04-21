#ifndef DETECTIONRESULTWINDOW_H
#define DETECTIONRESULTWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <vector>
#include <opencv2/opencv.hpp>

struct DetectedObject {
    cv::Rect boundingBox;
    cv::Mat image;
    int id;
    std::string className;
    float confidence;
};

class DetectionResultWindow : public QWidget
{
    Q_OBJECT

public:
    explicit DetectionResultWindow(const QString &title, QWidget *parent = nullptr);
    void setImage(const cv::Mat &image);
    void setDetectedObjects(const std::vector<DetectedObject> &objects);
    void updateDisplay();

signals:
    void objectSelected(int objectId);

private slots:
    void onPreviousClicked();
    void onNextClicked();
    void onObjectClicked(int objectId);

private:
    QLabel *m_imageLabel;
    QLabel *m_objectInfoLabel;
    QPushButton *m_previousBtn;
    QPushButton *m_nextBtn;
    QLabel *m_objectCountLabel;
    std::vector<DetectedObject> m_objects;
    int m_currentIndex;
    cv::Mat m_originalImage;
};

#endif // DETECTIONRESULTWINDOW_H
