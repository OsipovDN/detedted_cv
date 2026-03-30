#include "detection.h"

#include <fstream>
#include <iostream>  // ← Добавлено

namespace detection
{

Detection::Detection()
    : colors(
        {cv::Scalar(255, 255, 0), 
        cv::Scalar(0, 255, 0),
        cv::Scalar(0, 255, 255),
        cv::Scalar(255, 0, 0)})
    , model_shape(640, 640)
    , score_threshold(0.5f)
    , nms_threshold(0.5f) {}

std::vector<std::string> Detection::loadClassList() const
{
    std::vector<std::string> list{};
    std::ifstream ifs(coco_classes);
    
    if (!ifs.is_open()) {
        std::cerr << "Error: Cannot open file " << coco_classes << std::endl;
        return list;
    }
    
    std::string line{};
    while (std::getline(ifs, line)) {
        if (!line.empty()) {
            list.push_back(line);
        }
    }
    
    std::cout << "Loaded " << list.size() << " classes" << std::endl;
    return list;
}

void Detection::loadNet(
    cv::dnn::Net &net, 
    bool is_cuda) const
{
    auto res = cv::dnn::readNetFromONNX("../yolov8n.onnx");
    if (res.empty()) {
        std::cerr << "Error: Failed to load model!" << std::endl;
        return;  // ← Добавлено
    }
    
    if (is_cuda) {
        res.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
        res.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);
    } else {
        res.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
        res.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
    }

    net = res;
}

void Detection::detect(
    cv::Mat &image, 
    cv::dnn::Net &net, 
    std::vector<std::string> class_list) const
{
    if (image.empty()) return;
    
    cv::Mat model_input = image;  // ← Создаем копию

    cv::Mat blob;
    cv::dnn::blobFromImage(
        model_input, blob, 1.0/255.0, model_shape, cv::Scalar(), true, false);

    std::cout << "Blob dims: " << blob.dims << std::endl;
    for (int i = 0; i < blob.dims; i++) {
        std::cout << "Dim " << i << ": " << blob.size[i] << " ";
    }
    std::cout << std::endl;
    
    net.setInput(blob);
    cv::Mat prob = net.forward();

    std::vector<cv::Mat> outputs{};
    net.forward(outputs, net.getUnconnectedOutLayersNames());
    
    if (outputs.empty()) {
        std::cerr << "No outputs from network!" << std::endl;
        return;
    }

    //int rows = outputs[0].size[1];
    //int dimensions = outputs[0].size[2];

    int rows = outputs[0].size[2];
    int dimensions = outputs[0].size[1];

    outputs[0] = outputs[0].reshape(1, dimensions);
    cv::transpose(outputs[0], outputs[0]);

    float *data = (float*)outputs[0].data;

    std::vector<int> class_ids{};
    std::vector<float> confidences{};
    std::vector<cv::Rect> boxes{};

    for (int i = 0; i < rows; ++i)
    {
        float *classes_scores = data + 4;
        cv::Mat scores(1, 80, CV_32FC1, classes_scores);
        cv::Point class_id;
        double max_class_score;

        minMaxLoc(scores, nullptr, &max_class_score, nullptr, &class_id);

        if (max_class_score > score_threshold)
        {
            confidences.push_back(static_cast<float>(max_class_score));
            class_ids.push_back(class_id.x);

            float x = data[0];
            float y = data[1];
            float w = data[2];
            float h = data[3];

            //int left = static_cast<int>((x - w / 2));
            //int top = static_cast<int>((y - h / 2) );
            //int width = static_cast<int>(w );
            //int height = static_cast<int>(h);

            int left = static_cast<int>(x - 0.5*w)*model_input.cols/640.0;
            int top = static_cast<int>(y - 0.5*h)*model_input.rows/640.0;
            int width = static_cast<int>(w);
            int height = static_cast<int>(h);

            left = std::max(0, left);
            top = std::max(0, top);
            width = std::min(width, model_input.cols - left);
            height = std::min(height, model_input.rows - top);

            boxes.push_back(cv::Rect(left, top, width, height));
        }
        data += dimensions;
    }

    std::vector<int> nms_result;
    cv::dnn::NMSBoxes(boxes, confidences, score_threshold, nms_threshold, nms_result);

    for (size_t i = 0; i < nms_result.size(); ++i)
    {
        int idx = nms_result[i];
        if (idx < boxes.size() && idx < class_ids.size() && idx < confidences.size()) {
            cv::rectangle(model_input, boxes[idx], cv::Scalar(0, 255, 255), 2);
            cv::putText(
                model_input,
                std::to_string(int(confidences[idx] * 100)) + "%" + class_list[class_ids[idx]],
                cv::Point(boxes[idx].x, boxes[idx].y - 5),
                cv::FONT_HERSHEY_SIMPLEX,
                0.6,
                cv::Scalar(0, 255, 255),
                2
            );
        }
    }
    
    image = model_input;
}

}