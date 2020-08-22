#pragma once
#ifndef CHARACTERISTIC_220820_H
#define CHARACTERISTIC_220820_H


class Characteristic {

public:
    Characteristic(int vertical, int mainDiagonal, int horizontal, int sideDiagonal);

    int getVertical();
    void setVertical(int num);

    int getMainDiagonal();
    void setMainDiagonal(int num);

    int getHorizontal();
    void setHorizontal(int num);

    int getSideDiagonal();
    void setSideDiagonal(int num);

    int getCharacteristicByOrder(int order);

private:
    int vertical_, mainDiagonal_, horizontal_, sideDiagonal_;

    static bool comp2(int a, int b) { return (a > b); }
};
// TODO: add headers that you want to pre-compile here

#endif //CHARACTERISTIC_220820_H