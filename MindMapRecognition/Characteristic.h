#pragma once
#ifndef CHARACTERISTIC_220820_H
#define CHARACTERISTIC_220820_H


class Characteristic {

public:
    Characteristic();

    //pixel leng
    int getVerticalPixLeng();
    void setVerticalPixLeng(int num);

    int getMainDiagonalPixLeng();
    void setMainDiagonalPixLeng(int num);

    int getHorizontalPixLeng();
    void setHorizontalPixLeng(int num);

    int getSideDiagonalPixLeng();
    void setSideDiagonalPixLeng(int num);

    int getUpSideDiagonalPixLeng();
    void setUpSideDiagonalPixLeng(int num);

    int getDownSideDiagonalPixLeng();
    void setDownSideDiagonalPixLeng(int num);

    int getUpMainDiagonalPixLeng();
    void setUpMainDiagonalPixLeng(int num);

    int getDownMainDiagonalPixLeng();
    void setDownMainDiagonalPixLeng(int num);

    //leng
    int getVerticalLeng();

    int getMainDiagonalLeng();

    int getHorizontalLeng();

    int getSideDiagonalLeng();

    int getUpSideDiagonalLeng();

    int getDownSideDiagonalLeng();

    int getUpMainDiagonalLeng();

    int getDownMainDiagonalLeng();

    //other
    int getCharacteristicByOrder(int order);

private:
    int verticalPixLeng_, mainDiagonalPixLeng_, horizontalPixLeng_, sideDiagonalPixLeng_;
    int upMainDiagonalPixLeng_, downMainDiagonalPixLeng_, upSideDiagonalPixLeng_, downSideDiagonalPixLeng_;

    int verticalLeng_, mainDiagonalLeng_, horizontalLeng_, sideDiagonalLeng_;
    int upMainDiagonalLeng_, downMainDiagonalLeng_, upSideDiagonalLeng_, downSideDiagonalLeng_;

    static bool comp2(int a, int b) { return (a > b); }
};
// TODO: add headers that you want to pre-compile here

#endif //CHARACTERISTIC_220820_H