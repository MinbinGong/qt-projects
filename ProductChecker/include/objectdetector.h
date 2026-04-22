#ifndef OBJECTDETECTOR_H
#define OBJECTDETECTOR_H

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <vector>
#include <string>
#include "detectionresultwindow.h"

class ObjectDetector {
public:
    ObjectDetector();
    ~ObjectDetector();
    
    bool loadModel(const std::string& modelPath, const std::string& configPath, const std::string& classesPath);
    std::vector<DetectedObject> detectObjects(const cv::Mat& image, float confidenceThreshold = 0.5);
    
private:
    cv::dnn::Net net;
    std::vector<std::string> classes;
    
    void preprocessImage(const cv::Mat& image, cv::Mat& blob);
    void postprocessImage(const cv::Mat& image, const std::vector<cv::Mat>& outputs, std::vector<DetectedObject>& detectedObjects, float confidenceThreshold);
    std::vector<DetectedObject> detectObjectsUsingContours(const cv::Mat& image);
};

#endif // OBJECTDETECTOR_H
