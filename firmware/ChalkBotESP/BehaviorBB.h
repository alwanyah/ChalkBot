#ifndef BEHAVIOR_BB_H
#define BEHAVIOR_BB_H

class BehaviorBB {
    friend class BehaviorClass;

    bool moving = false;

public:
    bool isMoving() const {
        return moving;
    }
};

#endif
