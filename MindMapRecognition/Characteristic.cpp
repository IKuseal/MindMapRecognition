
#include "Characteristic.h"
#include <vector>
#include <algorithm>

Characteristic::Characteristic() :
    verticalPixLeng_(0), mainDiagonalPixLeng_(0), horizontalPixLeng_(0), sideDiagonalPixLeng_(0),
    upMainDiagonalPixLeng_(0), downMainDiagonalPixLeng_(0), upSideDiagonalPixLeng_(0), downSideDiagonalPixLeng_(0),

    verticalLeng_(0), mainDiagonalLeng_(0), horizontalLeng_(0), sideDiagonalLeng_(0),
    upMainDiagonalLeng_(0), downMainDiagonalLeng_(0), upSideDiagonalLeng_(0), downSideDiagonalLeng_(0)
{
};

//pixel leng
void Characteristic::setVerticalPixLeng(int num) {
    verticalPixLeng_ = num;
    verticalLeng_ = verticalPixLeng_;
}

int Characteristic::getVerticalPixLeng() {
    return verticalPixLeng_;
}

void Characteristic::setHorizontalPixLeng(int num) {
    horizontalPixLeng_ = num;
    horizontalLeng_ = horizontalPixLeng_;
}

int Characteristic::getHorizontalPixLeng() {
    return horizontalPixLeng_;
}

void Characteristic::setMainDiagonalPixLeng(int num) {
    mainDiagonalPixLeng_ = num;
    mainDiagonalLeng_ = mainDiagonalPixLeng_ * sqrt(2);
}

int Characteristic::getMainDiagonalPixLeng() {
    return mainDiagonalPixLeng_;
}

void Characteristic::setSideDiagonalPixLeng(int num) {
    sideDiagonalPixLeng_ = num;
    sideDiagonalLeng_ = sideDiagonalPixLeng_ * sqrt(2);
}

int Characteristic::getSideDiagonalPixLeng() {
    return sideDiagonalPixLeng_;
}

int Characteristic::getUpSideDiagonalPixLeng() {
    return upSideDiagonalPixLeng_;
}
void Characteristic::setUpSideDiagonalPixLeng(int num) {
    upSideDiagonalPixLeng_ = num;
}

int Characteristic::getDownSideDiagonalPixLeng() {
    return downSideDiagonalPixLeng_;
}
void Characteristic::setDownSideDiagonalPixLeng(int num) {
    downSideDiagonalPixLeng_ = num;
}

int Characteristic::getUpMainDiagonalPixLeng() {
    return upMainDiagonalPixLeng_;
}
void Characteristic::setUpMainDiagonalPixLeng(int num) {
    upMainDiagonalPixLeng_ = num;
}

int Characteristic::getDownMainDiagonalPixLeng() {
    return downMainDiagonalPixLeng_;
}
void Characteristic::setDownMainDiagonalPixLeng(int num) {
    downMainDiagonalPixLeng_ = num;
}

//leng

int Characteristic::getVerticalLeng() {
    return verticalLeng_;
}

int Characteristic::getHorizontalLeng() {
    return horizontalLeng_;
}

int Characteristic::getMainDiagonalLeng() {
    return mainDiagonalLeng_;
}

int Characteristic::getSideDiagonalLeng() {
    return sideDiagonalLeng_;
}

int Characteristic::getUpSideDiagonalLeng() {
    return upSideDiagonalLeng_;
}

int Characteristic::getDownSideDiagonalLeng() {
    return downSideDiagonalLeng_;
}

int Characteristic::getUpMainDiagonalLeng() {
    return upMainDiagonalLeng_;
}

int Characteristic::getDownMainDiagonalLeng() {
    return downMainDiagonalLeng_;
}


//other
int Characteristic::getCharacteristicByOrder(int order) {
    
    std::vector<int> arr{ verticalLeng_, mainDiagonalPixLeng_, horizontalLeng_, sideDiagonalLeng_,
    upMainDiagonalPixLeng_,downMainDiagonalPixLeng_,downSideDiagonalPixLeng_,upSideDiagonalPixLeng_};

    //std::vector<int> arr{ verticalLeng_, mainDiagonalPixLeng_, horizontalLeng_, sideDiagonalLeng_};

    sort(arr.begin(), arr.end(), comp2);
   
    return arr.at(order);
}

