#include "objectdetector.h"
#include <fstream>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

ObjectDetector::ObjectDetector() {
}

ObjectDetector::~ObjectDetector() {
}

bool ObjectDetector::loadModel(const std::string& modelPath, const std::string& configPath, const std::string& classesPath) {
    try {
        // 检查文件是否存在
        if (!fs::exists(modelPath)) {
            std::cerr << "Model file not found: " << modelPath << std::endl;
            return false;
        }
        if (!fs::exists(configPath)) {
            std::cerr << "Config file not found: " << configPath << std::endl;
            return false;
        }
        if (!fs::exists(classesPath)) {
            std::cerr << "Classes file not found: " << classesPath << std::endl;
            return false;
        }
        
        // 加载模型
        net = cv::dnn::readNetFromDarknet(configPath, modelPath);
        
        // 设置后端和目标
        net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
        net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
        
        // 加载类别名称
        std::ifstream classesFile(classesPath);
        if (!classesFile.is_open()) {
            std::cerr << "Error opening classes file" << std::endl;
            return false;
        }
        
        std::string className;
        while (std::getline(classesFile, className)) {
            classes.push_back(className);
        }
        
        std::cout << "Model loaded successfully: " << modelPath << std::endl;
        std::cout << "Config loaded successfully: " << configPath << std::endl;
        std::cout << "Classes loaded successfully: " << classesPath << std::endl;
        std::cout << "Number of classes: " << classes.size() << std::endl;
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading model: " << e.what() << std::endl;
        return false;
    }
}

void ObjectDetector::preprocessImage(const cv::Mat& image, cv::Mat& blob) {
    // 预处理图像，调整大小并归一化
    cv::dnn::blobFromImage(image, blob, 1/255.0, cv::Size(416, 416), cv::Scalar(0, 0, 0), true, false);
}

void ObjectDetector::postprocessImage(const cv::Mat& image, const std::vector<cv::Mat>& outputs, std::vector<DetectedObject>& detectedObjects, float confidenceThreshold) {
    std::vector<int> classIds;
    std::vector<float> confidences;
    std::vector<cv::Rect> boxes;
    
    // 处理每个输出层
    for (size_t i = 0; i < outputs.size(); ++i) {
        float* data = (float*)outputs[i].data;
        for (int j = 0; j < outputs[i].rows; ++j, data += outputs[i].cols) {
            // 获取类别得分
            cv::Mat scores = outputs[i].row(j).colRange(5, outputs[i].cols);
            cv::Point classIdPoint;
            double confidence;
            cv::minMaxLoc(scores, 0, &confidence, 0, &classIdPoint);
            
            // 过滤低置信度的检测结果
            if (confidence > confidenceThreshold) {
                // 计算边界框坐标
                int centerX = (int)(data[0] * image.cols);
                int centerY = (int)(data[1] * image.rows);
                int width = (int)(data[2] * image.cols);
                int height = (int)(data[3] * image.rows);
                int left = centerX - width / 2;
                int top = centerY - height / 2;
                
                // 边界检查，确保矩形在图像范围内
                left = std::max(0, left);
                top = std::max(0, top);
                width = std::min(width, image.cols - left);
                height = std::min(height, image.rows - top);
                
                // 确保宽度和高度为正数
                if (width > 0 && height > 0) {
                    classIds.push_back(classIdPoint.x);
                    confidences.push_back((float)confidence);
                    boxes.push_back(cv::Rect(left, top, width, height));
                }
            }
        }
    }
    
    // 非极大值抑制，去除重叠的边界框
    std::vector<int> indices;
    cv::dnn::NMSBoxes(boxes, confidences, confidenceThreshold, 0.4, indices);
    
    // 生成检测结果
    int objectId = 0;
    for (size_t i = 0; i < indices.size(); ++i) {
        int idx = indices[i];
        DetectedObject obj;
        obj.boundingBox = boxes[idx];
        obj.image = image(obj.boundingBox).clone();
        obj.id = objectId++;
        
        // 确保类别索引有效
        if (classIds[idx] >= 0 && classIds[idx] < classes.size()) {
            obj.className = classes[classIds[idx]];
        } else {
            obj.className = "unknown";
        }
        
        obj.confidence = confidences[idx];
        
        detectedObjects.push_back(obj);
    }
    
    std::cout << "Detected objects: " << detectedObjects.size() << std::endl;
}

std::vector<DetectedObject> ObjectDetector::detectObjects(const cv::Mat& image, float confidenceThreshold) {
    std::vector<DetectedObject> detectedObjects;
    
    if (image.empty()) {
        std::cerr << "Empty image provided for detection" << std::endl;
        return detectedObjects;
    }
    
    try {
        if (net.empty()) {
            std::cerr << "Model not loaded, falling back to contour-based detection" << std::endl;
            // 回退到基于轮廓的物品检测方法
            return detectObjectsUsingContours(image);
        }
        
        // 预处理图像
        cv::Mat blob;
        preprocessImage(image, blob);
        
        // 前向传播
        net.setInput(blob);
        std::vector<cv::Mat> outputs;
        net.forward(outputs, net.getUnconnectedOutLayersNames());
        
        // 后处理检测结果
        postprocessImage(image, outputs, detectedObjects, confidenceThreshold);
    } catch (const std::exception& e) {
        std::cerr << "Error detecting objects: " << e.what() << std::endl;
        // 发生错误时，回退到基于轮廓的物品检测方法
        return detectObjectsUsingContours(image);
    }
    
    return detectedObjects;
}

std::vector<DetectedObject> ObjectDetector::detectObjectsUsingContours(const cv::Mat& image) {
    std::vector<DetectedObject> detectedObjects;
    
    try {
        // 转换为灰度图像
        cv::Mat gray;
        cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);

        // 高斯模糊，减少噪声
        cv::GaussianBlur(gray, gray, cv::Size(5, 5), 0);

        // 自适应阈值，适应不同光照条件
        cv::Mat thresh;
        cv::adaptiveThreshold(gray, thresh, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV, 11, 2);

        // 查找轮廓
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(thresh, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        // 处理轮廓
        int objectId = 0;
        for (size_t i = 0; i < contours.size(); i++) {
            double area = cv::contourArea(contours[i]);
            // 降低面积阈值，检测更小的物品
            if (area < 100) {
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
        
        std::cout << "Contour-based detection found " << detectedObjects.size() << " objects" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error in contour-based detection: " << e.what() << std::endl;
    }
    
    return detectedObjects;
}
