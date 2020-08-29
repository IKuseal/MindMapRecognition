#pragma once

#ifndef MAPITERATOR_220820_H
#define MAPITERATOR_220820_H

#include <queue>
#include "myNode.h"
using namespace std;


class MapIterator {
public:
    MapIterator(MyNode head);
    bool atEnd();
    MyNode next();
private:
    queue<MyNode> processed;
};

#endif //MAPITERATOR_220820_H