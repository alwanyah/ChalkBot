#ifndef ODOMETRY_BB_H
#define ODOMETRY_BB_H

#include "math/Pose2D.h"

class OdometryBB {
    friend class OdometryClass;

    Pose2D robotPose;

public:
    Pose2D getRobotPose() const {
        return robotPose;
    }
};

#endif
