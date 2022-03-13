#ifndef POSE_FUSION_BB_H
#define POSE_FUSION_BB_H

#include <Arduino.h>
#include "../util/math/Pose2D.h"

class PoseFusionBB {
    friend class PoseFusion;

    Pose2D fusedPose;
    bool useGnss = false;

public:
    // translation north/west (mm), rotation counterclockwise (rad)
    Pose2D getFusedPose() const {
        return fusedPose;
    }

    // translation north (m)
    double getNorth() const {
        return fusedPose.translation.x / 1000.0;
    }

    // translation east (m)
    double getEast() const {
        return fusedPose.translation.y / -1000.0;
    }

    // rotation clockwise (rad)
    double getHeading() const {
        return -fusedPose.rotation;
    }

    bool isUsingGnss() const {
        return useGnss;
    }

    void setUseGnss(bool use) {
        useGnss = use;
    }
};

#endif
