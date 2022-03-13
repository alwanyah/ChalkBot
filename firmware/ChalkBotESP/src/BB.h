#ifndef BB_H
#define BB_H

// Blackboard.

#include "BB/ImuBB.h"
#include "BB/GnssBB.h"
#include "BB/OdometryBB.h"
#include "BB/PoseFusionBB.h"
#include "BB/ChalkbotMotorDriverBB.h"
#include "BB/ChalkbotMotorControllerBB.h"
#include "BB/PrintDriverBB.h"
#include "BB/NetBB.h"
#include "BB/WebServerBB.h"
#include "BB/BehaviorBB.h"

namespace bb {
    extern ImuBB imu;
    extern GnssBB gnss;
    extern OdometryBB odometry;
    extern PoseFusionBB poseFusion;
    extern ChalkbotMotorDriverBB chalkbotMotorDriver;
    extern ChalkbotMotorControllerBB chalkbotMotorController;
    extern PrintDriverBB printDriver;
    extern NetBB net;
    extern WebServerBB webServer;
    extern BehaviorBB behavior;
}

#endif
