#pragma once

#ifndef NODE_220820_H
#define NODE_220820_H

#include <vector>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

class MyNode {
public:
    MyNode();
    MyNode(int id);
    
    int id{ -1 };
    
    std::vector<MyNode> children;

    cv::Point lineBegPoint, lineEndPoint;
};


#endif //NODE_220820_H