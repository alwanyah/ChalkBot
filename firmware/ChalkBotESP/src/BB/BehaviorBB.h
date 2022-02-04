#ifndef BEHAVIOR_BB_H
#define BEHAVIOR_BB_H

#include <Arduino.h>
#include <vector>
#include <utility>
#include "../util/Action.h"

class BehaviorBB {
    friend class Behavior;

    bool moving = false;

    std::vector<Action> actions;
    bool actionInProgress = false;

public:
    bool isMoving() const {
        return moving;
    }

    const std::vector<Action> &getActions() const {
        return actions;
    }

    void setActions(const std::vector<Action> &new_actions) {
        actions = new_actions;
        actionInProgress = false;
    }

    void setActions(std::vector<Action> &&new_actions) {
        actions = std::move(new_actions);
        actionInProgress = false;
    }

    void pushActions(const std::vector<Action> &new_actions) {
        actions.insert(actions.end(), new_actions.begin(), new_actions.end());
    }
};

#endif
