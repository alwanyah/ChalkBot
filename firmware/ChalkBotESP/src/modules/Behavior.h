#ifndef BEHAVIOR_H
#define BEHAVIOR_H

#include <Arduino.h>
#include "../util/Action.h"

class Behavior {
    unsigned long actionStartedAt; // ms

public:
    void update();
    void legacyBehavior();
    void nextAction();
    void drive(const ActionDrive &action, unsigned long elapsed);
    void goto_(const ActionGoto &action, unsigned long elapsed);
};

#endif
