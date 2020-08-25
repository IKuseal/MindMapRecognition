#include "Neighbors.h"

NeighborsCharacteristic::NeighborsCharacteristic(){}

void NeighborsCharacteristic::setIsNodeNeighbor(bool value) {
    isNodeNeighbor = value;
}

bool NeighborsCharacteristic::getIsNodeNeighbor() {
    return isNodeNeighbor;
}

void NeighborsCharacteristic::setIsLineNeighbor(bool value) {
    isLineNeighbor = value;
}

bool NeighborsCharacteristic::getIsLineNeighbor() {
    return isLineNeighbor;
}