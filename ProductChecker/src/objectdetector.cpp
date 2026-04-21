#include "objectdetector.h"
#include <fstream>
#include <iostream>

ObjectDetector::ObjectDetector() {
}

ObjectDetector::~ObjectDetector() {
}

bool ObjectDetector::loadModel(const std::string& modelPath, const std::string& configPath, const std::string& classesPath) {
    try {
        net = cv::dnn::readNetFromDarknet(configPath, modelPath);
        net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
        net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
        
        std::ifstream classesFile(classesPath);
        if (!classesFile.is_open()) {
            std::cerr << "Error opening classes file" << std::endl;
            return false;
        }
        
        std::string className;
        while (std::getline(classesFile, className)) {
            classes.push_back(className);
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading model: " << e.what() << std::endl;
        return false;
    }
}

void ObjectDetector::preprocessImage(const cv::Mat& image, cv::Mat& blob) {
    cv::dnn::blobFromImage(image, blob, 1/255.0, cv::Size(416, 416), cv::Scalar(0, 0, 0), true, false);
}

void ObjectDetector::postprocessImage(const cv::Mat& image, const std::vector<cv::Mat>& outputs, std::vector<DetectedObject>& detectedObjects, float confidenceThreshold) {
    std::vector<int> classIds;
    std::vector<float> confidences;
    std::vector<cv::Rect> boxes;
    
    for (size_t i = 0; i < outputs.size(); ++i) {
        float* data = (float*)outputs[i].data;
        for (int j = 0; j < outputs[i].rows; ++j, data += outputs[i].cols) {
            cv::Mat scores = outputs[i].row(j).colRange(5, outputs[i].cols);
            cv::Point classIdPoint;
            double confidence;
            cv::minMaxLoc(scores, 0, &confidence, 0, &classIdPoint);
            
            if (confidence > confidenceThreshold) {
                int centerX = (int)(data[0] * image.cols);
                int centerY = (int)(data[1] * image.rows);
                int width = (int)(data[2] * image.cols);
                int height = (int)(data[3] * image.rows);
                int left = centerX - width / 2;
                int top = centerY - height / 2;
                
                classIds.push_back(classIdPoint.x);
                confidences.push_back((float)confidence);
                boxes.push_back(cv::Rect(left, top, width, height));
            }
        }
    }
    
    std::vector<int> indices;
    cv::dnn::NMSBoxes(boxes, confidences, confidenceThreshold, 0.4, indices);
    
    int objectId = 0;
    for (size_t i = 0; i < indices.size(); ++i) {
        int idx = indices[i];
        DetectedObject obj;
        obj.boundingBox = boxes[idx];
        obj.image = image(obj.boundingBox).clone();
        obj.id = objectId++;
        obj.className = classes[classIds[idx]];
        obj.confidence = confidences[idx];
        
        detectedObjects.push_back(obj);
    }
}

std::vector<DetectedObject> ObjectDetector::detectObjects(const cv::Mat& image, float confidenceThreshold) {
    std::vector<DetectedObject> detectedObjects;
    
    if (image.empty()) {
        return detectedObjects;
    }
    
    try {
        if (net.empty()) {
            // 回退到基于轮廓的物品检测方法
            cv::Mat gray;
            cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);

            cv::GaussianBlur(gray, gray, cv::Size(5, 5), 0);

            cv::Mat thresh;
            cv::threshold(gray, thresh, 127, 255, cv::THRESH_BINARY);

            std::vector<std::vector<cv::Point>> contours;
            cv::findContours(thresh, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

            int objectId = 0;
            for (size_t i = 0; i < contours.size(); i++) {
                double area = cv::contourArea(contours[i]);
                if (area < 500) {
                    continue;
                }

                DetectedObject obj;
                obj.boundingBox = cv::boundingRect(contours[i]);
                obj.image = image(obj.boundingBox).clone();
                obj.id = objectId++;
                obj.className = "object";
                obj.confidence = 1.0;

                detectedObjects.push_back(obj);
            }
            return detectedObjects;
        }
        
        cv::Mat blob;
        preprocessImage(image, blob);
        
        net.setInput(blob);
        std::vector<cv::Mat> outputs;
        net.forward(outputs, net.getUnconnectedOutLayersNames());
        
        postprocessImage(image, outputs, detectedObjects, confidenceThreshold);
    } catch (const std::exception& e) {
        std::cerr << "Error detecting objects: " << e.what() << std::endl;
        // 发生错误时，回退到基于轮廓的物品检测方法
        cv::Mat gray;
        cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);

        cv::GaussianBlur(gray, gray, cv::Size(5, 5), 0);

        cv::Mat thresh;
        cv::threshold(gray, thresh, 127, 255, cv::THRESH_BINARY);

        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(thresh, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        int objectId = 0;
        for (size_t i = 0; i < contours.size(); i++) {
            double area = cv::contourArea(contours[i]);
            if (area < 500) {
                continue;
            }

            DetectedObject obj;
            obj.boundingBox = cv::boundingRect(contours[i]);
            obj.image = image(obj.boundingBox).clone();
            obj.id = objectId++;
            obj.className = "object";
            obj.confidence = 1.0;

            detectedObjects.push_back(obj);
        }
    }
    
    return detectedObjects;
}
