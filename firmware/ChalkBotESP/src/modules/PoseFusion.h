#ifndef POSE_FUSION_H
#define POSE_FUSION_H

#include <Arduino.h>
#include "../util/math/Pose2D.h"

class PoseFusion {
    Pose2D lastOdometry;
    unsigned long lastGnssTimestamp = 0;

public:
    void update();
};

#endif
