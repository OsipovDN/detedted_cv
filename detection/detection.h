#include "opencv2/opencv.hpp"

#include <vector>

namespace core::detection{

    struct DetectionResult {
        std::vector<cv::Rect> boxes;
        std::vector<int> classIds;
        std::mutex mtx;
    };  

class Detection{
public: 
    explicit Detection();
    std::vector<std::string> loadClassList() const;

    void loadNet(
        cv::dnn::Net &net, 
        bool is_cuda = false) const;

    void detect(
        cv::Mat &image, 
        cv::dnn::Net &net, 
        std::vector<std::string> class_list) const;

private:
    const std::vector<cv::Scalar>   colors;
    cv::Size                        model_shape;
    float                           score_threshold;
    float                           nms_threshold;

    const std::string               model_path = "./model/yolov8n.onnx";
    const std::string               coco_classes = "./model/coco";
};

}