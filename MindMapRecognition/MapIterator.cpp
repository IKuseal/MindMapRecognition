#include "MapIterator.h"

MapIterator::MapIterator(MyNode head) {
    processed = queue<MyNode>();
    processed.push(head);
}

bool MapIterator::atEnd() {
    return processed.empty();
}

MyNode MapIterator::next() {
    MyNode node = processed.front();
    for (int i = 0; i < node.children.size(); ++i) {
        processed.push(node.children.at(i));
    }
    processed.pop();
    return node;
}