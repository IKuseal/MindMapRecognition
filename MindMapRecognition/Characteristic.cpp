
#include "Characteristic.h"
#include <vector>
#include <algorithm>

Characteristic::Characteristic(int vertical, int mainDiagonal, int horizontal, int sideDiagonal) :
    vertical_(vertical), mainDiagonal_(mainDiagonal), horizontal_(horizontal), sideDiagonal_(sideDiagonal)
{
};

void Characteristic::setVertical(int num) {
    vertical_ = num;
}

int Characteristic::getVertical() {
    return vertical_;
}

void Characteristic::setHorizontal(int num) {
    horizontal_ = num;
}

int Characteristic::getHorizontal() {
    return horizontal_;
}

void Characteristic::setMainDiagonal(int num) {
    mainDiagonal_ = num;
}

int Characteristic::getMainDiagonal() {
    return mainDiagonal_;
}

void Characteristic::setSideDiagonal(int num) {
    sideDiagonal_ = num;
}

int Characteristic::getSideDiagonal() {
    return sideDiagonal_;
}

int Characteristic::getCharacteristicByOrder(int order) {
    
    std::vector<int> arr{ vertical_, mainDiagonal_, horizontal_, sideDiagonal_ };

    sort(arr.begin(), arr.end(), comp2);
   
    return arr.at(order);
}

