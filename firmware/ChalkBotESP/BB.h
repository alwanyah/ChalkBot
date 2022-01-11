#ifndef BB_H
#define BB_H

// Blackboard.

#include "ImuBB.h"
#include "GnssBB.h"
#include "OdometryBB.h"
#include "ChalkbotMotorBB.h"
#include "NetBB.h"
#include "WebServerBB.h"
#include "BehaviorBB.h"

namespace bb {
    extern ImuBB imu;
    extern GnssBB gnss;
    extern OdometryBB odometry;
    extern ChalkbotMotorBB chalkbotMotor;
    extern NetBB net;
    extern WebServerBB webServer;
    extern BehaviorBB behavior;
}

#endif
