#pragma once

#ifndef NEIGHBORS_220820_H
#define NEIGHBORS_220820_H

class NeighborsCharacteristic {
public:
    NeighborsCharacteristic();

    void setIsNodeNeighbor(bool value);
    bool getIsNodeNeighbor();

    void setIsLineNeighbor(bool value);
    bool getIsLineNeighbor();
private:
    bool isNodeNeighbor{ false };
    bool isLineNeighbor{ false };

};


#endif //CHARACTERISTIC_220820_H