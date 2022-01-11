#include "ChalkbotMotor.h"
#include "Config.h"

ChalkbotMotorClass ChalkbotMotor(
    { config::pins::motorFrontLeftPwm, config::pins::motorFrontLeftDir },
    { config::pins::motorFrontRightPwm, config::pins::motorFrontRightDir },
    { config::pins::motorRearLeftPwm, config::pins::motorRearLeftDir },
    { config::pins::motorRearRightPwm, config::pins::motorRearRightDir }
);

