#include "PoseFusion.h"
#include "../BB.h"
#include "../Config.h"

// TODO: use filter

void PoseFusion::update() {
    const Pose2D currentOdometry = bb::odometry.getRobotPose();
    const Pose2D odomertyDelta = currentOdometry - lastOdometry;

    bb::poseFusion.fusedPose += odomertyDelta;

    const unsigned long currentGnssTimestamp = bb::gnss.getRelativeTimestamp();
    if (bb::poseFusion.useGnss && currentGnssTimestamp != lastGnssTimestamp) {
        const double gnssNorth = bb::gnss.getNorth();
        const double gnssEast = bb::gnss.getEast();
        const double gnssNorthAccuracy = bb::gnss.getNorthAccuracy();
        const double gnssEastAccuracy = bb::gnss.getEastAccuracy();

        double poseNorth = bb::poseFusion.getNorth();
        double poseEast = bb::poseFusion.getEast();

        if (poseNorth > gnssNorth + gnssNorthAccuracy * config::fusion::MAX_DEVIATION_FACTOR) {
            poseNorth = gnssNorth + gnssNorthAccuracy * config::fusion::ADJUSTMENT_TARGET_FACTOR;
        } else if (poseNorth < gnssNorth - gnssNorthAccuracy * config::fusion::MAX_DEVIATION_FACTOR) {
            poseNorth = gnssNorth - gnssNorthAccuracy * config::fusion::ADJUSTMENT_TARGET_FACTOR;
        }

        if (poseEast > gnssEast + gnssEastAccuracy * config::fusion::MAX_DEVIATION_FACTOR) {
            poseEast = gnssEast + gnssEastAccuracy * config::fusion::ADJUSTMENT_TARGET_FACTOR;
        } else if (poseEast < gnssEast - gnssEastAccuracy * config::fusion::MAX_DEVIATION_FACTOR) {
            poseEast = gnssEast - gnssEastAccuracy * config::fusion::ADJUSTMENT_TARGET_FACTOR;
        }

        bb::poseFusion.fusedPose.translation.x = poseNorth * 1000.0;
        bb::poseFusion.fusedPose.translation.y = poseEast * -1000.0;

        lastGnssTimestamp = currentGnssTimestamp;
    }

    lastOdometry = currentOdometry;
}
