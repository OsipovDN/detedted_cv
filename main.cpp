#include <mutex>
#include <fstream>
#include <iostream>  // ← Добавлено
#include <vector>

#include "detection.h"


int main()
{
    auto detection = detection::Detection();
    const auto  class_list = detection.loadClassList();
    
    cv::VideoCapture capture(0, cv::CAP_V4L2);
    if (!capture.isOpened()) { 
        std::cerr << "Error: Cannot open camera!" << std::endl;
        return -1;
    }

    cv::dnn::Net net;
    detection.loadNet(net);
    
    if (net.empty()) {  // ← Добавлена проверка
        std::cerr << "Error: Network is empty!" << std::endl;
        return -1;
    }

    cv::Mat frame;
    cv::namedWindow("output", cv::WINDOW_NORMAL); 
    
    while (true)
    {
        capture.read(frame);
        if (frame.empty()) {
            std::cerr << "Empty frame!" << std::endl;
            break;
        }
        
       detection.detect(frame, net, class_list);
        
        cv::imshow("output", frame);
        if (cv::waitKey(1) == 27) {  // ESC для выхода
            break;
        }
    }

    capture.release();
    cv::destroyAllWindows();
    
    return 0;
}