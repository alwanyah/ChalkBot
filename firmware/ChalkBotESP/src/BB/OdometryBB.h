#ifndef ODOMETRY_BB_H
#define ODOMETRY_BB_H

#include <Arduino.h>
#include "../util/math/Pose2D.h"

class OdometryBB {
    friend class Odometry;

    Pose2D robotPose;

public:
    Pose2D getRobotPose() const {
        return robotPose;
    }
};

#endif
