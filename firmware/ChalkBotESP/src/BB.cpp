#include "BB.h"

namespace bb {
    ImuBB imu;
    GnssBB gnss;
    OdometryBB odometry;
    PoseFusionBB poseFusion;
    ChalkbotMotorDriverBB chalkbotMotorDriver;
    ChalkbotMotorControllerBB chalkbotMotorController;
    PrintDriverBB printDriver;
    NetBB net;
    WebServerBB webServer;
    BehaviorBB behavior;
}
