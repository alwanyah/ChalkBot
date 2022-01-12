#ifndef BEHAVIOR_BB_H
#define BEHAVIOR_BB_H

#include <Arduino.h>

class BehaviorBB {
    friend class Behavior;

    bool moving = false;

public:
    bool isMoving() const {
        return moving;
    }
};

#endif
