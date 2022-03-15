#include "Behavior.h"
#include "../BB.h"
#include "../util/Logger.h"

static Logger logger("behavior");

// FIXME: Convert all Serial.print to logger.log


// FIXME: extract this into a different module
// controller for closed loop rotation control
static void driveWithTimeIMU(float x, float angle, int printSpeed)
{
    // diff angle to turn
    double diff = Math::normalize(angle - bb::imu.getOrientation());

    // P-controller
    float va = (float)(diff*1024.0f);

    // constraint max velocity
    float maxRotationPWM = 64.0f;
    va = constrain(va, -maxRotationPWM, maxRotationPWM);

    // FIXME: log
    // if(chalkWebServer.debug == 2) {
    //     Serial.print(va);
    // }

    // continuous correction
    bb::chalkbotMotorController.setVelocitySmooth(x, va);

    // prioritize turn correction
    /*
    if(abs(va) < maxRotationPWM) {
        ChalkbotMotor.setVelocitySmooth(x, va);
    } else {
        ChalkbotMotor.setVelocitySmooth(0, va);
    }
    */

    // print
    bb::printDriver.setSpeed(printSpeed);
}

void Behavior::update() {
    unsigned long now = millis();
    if (!bb::behavior.actionInProgress) {
        if (bb::behavior.actions.empty()) {
            legacyBehavior();
            return;
        }
        bb::behavior.actionInProgress = true;
        actionStartedAt = now;
    }
    unsigned long elapsed = now - actionStartedAt;
    const Action &action = bb::behavior.actions.front();
    switch (action.getType()) {
        case Action::DRIVE:
            drive(action.getDrive(), elapsed);
            break;

        case Action::GOTO:
            goto_(action.getGoto(), elapsed);
            break;

        case Action::ERROR:
            throw BadActionType();
    }
}

void Behavior::nextAction() {
    bb::chalkbotMotorController.setVelocitySmooth(0, 0);
    bb::printDriver.setSpeed(0);
    bb::behavior.actions.erase(bb::behavior.actions.begin());
    bb::behavior.actionInProgress = false;
}

void Behavior::drive(const ActionDrive &action, unsigned long elapsed) {
    if (elapsed / 1000.0 >= action.duration) {
        nextAction();
        return;
    }

    uint16_t x_pwm = static_cast<uint16_t>(action.forward * 255.0);
    uint16_t a_pwm = static_cast<uint16_t>(action.clockwise * -255.0);
    uint16_t p_pwm = static_cast<uint16_t>(action.print * 255.0);

    bb::chalkbotMotorController.setVelocitySmooth(x_pwm, a_pwm);
    bb::printDriver.setSpeed(p_pwm);

    logger.log_debug("DRIVE");
}

void Behavior::goto_(const ActionGoto &action, unsigned long elapsed) {
    (void)elapsed;

    // convert meters north/east -> millimeters north/west (FIXME: we shouldn't need this)
    Vector2d target(action.north * 1000.0, action.east * -1000.0);
    uint16_t p_pwm = static_cast<uint16_t>(action.print * 255.0); // relative (-1.0 ~ 1.0) -> PWM (-255 ~ 255)

    // FIXME: move to Config.h
    float maxRotationPWM = 64.0f;

    Pose2D robotPose = bb::poseFusion.getFusedPose();

    // transform the target from global coordinates into local coordinates of the robot
    Vector2d targetPointLocal = robotPose / target;

    // constraint max velocity
    float vx = constrain(targetPointLocal.abs(), -maxRotationPWM, maxRotationPWM);

    float maxDistanceError = 30;

    // Serial.println(vx);
    if(abs(vx) > maxDistanceError)
    {
        // P-controller
        float va = (float)(targetPointLocal.angle() * 1024.0f);
        va = constrain(va, -maxRotationPWM, maxRotationPWM);

        // if the direction is wrong, correct first
        if(abs(va) < maxRotationPWM) {
            bb::chalkbotMotorController.setVelocitySmooth(vx, va);
            // only print, when driving
            bb::printDriver.setSpeed(p_pwm);
        } else {
            bb::chalkbotMotorController.setVelocitySmooth(0, va);
        }
    }
    else
    {
        nextAction();
        return;
    }

    auto writer = logger.writer_debug();
    writer.print("GOTO: ");
    writer.print(targetPointLocal.x);
    writer.print(",");
    writer.print(targetPointLocal.y);
    writer.print(",");
    writer.print(p_pwm);
}

void Behavior::legacyBehavior() {
    unsigned long elapsed = millis() - bb::webServer.getTimeOfLastCommand();
    auto command = bb::webServer.getLastCommand();
    bool stop = false;
    switch (command) {
        case Command::MOTOR_TEST: {
            auto request = bb::webServer.getMotorTestRequest();
            if (elapsed >= 3000) {
                stop = true;
                break;
            }

            bb::chalkbotMotorController.disable();

            bb::chalkbotMotorDriver.setMotorVelocity(
                request.motorFrontLeftSpeed,
                request.motorFrontRightSpeed,
                request.motorRearLeftSpeed,
                request.motorRearRightSpeed
            );
            bb::printDriver.setSpeed(request.printerSpeed);

            bb::behavior.moving = true;
            break;
        }

        case Command::DRIVE: {
            auto request = bb::webServer.getDriveRequest();
            if (elapsed >= request.duration) {
                stop = true;
                break;
            }

            bb::chalkbotMotorController.setVelocitySmooth(request.x_pwm, request.a_pwm);
            bb::printDriver.setSpeed(request.p_pwm);

            logger.log_debug("DRIVE");
            bb::behavior.moving = true;
            break;
        }

        case Command::DRIVE_IMU: {
            auto request = bb::webServer.getDriveIMURequest();
            if (elapsed >= request.duration) {
                stop = true;
                break;
            }

            driveWithTimeIMU(
                request.vx,
                request.va,
                request.p
            );

            auto writer = logger.writer_debug();
            writer.print("DRIVE_IMU: ");
            writer.print(request.vx);
            writer.print(", ");
            writer.print(request.va);
            writer.print(", ");
            writer.print(request.p);
            writer.print(", ");
            writer.print(request.duration);
            writer.finish();

            bb::behavior.moving = true;
            break;
        }

        case Command::GOTO: {
            auto request = bb::webServer.getGotoPointRequest();

            // FIXME: move to Config.h
            float maxRotationPWM = 64.0f;

            Pose2D robotPose = bb::poseFusion.getFusedPose();

            // transform the target from global coordinates into local coordinates of the robot
            Vector2d targetPointLocal = robotPose / request.target;

            // constraint max velocity
            float vx = constrain(targetPointLocal.abs(), -maxRotationPWM, maxRotationPWM);

            float maxDistanceError = 30;

            // Serial.println(vx);
            if(abs(vx) > maxDistanceError)
            {
                // P-controller
                float va = (float)(targetPointLocal.angle() * 1024.0f);
                va = constrain(va, -maxRotationPWM, maxRotationPWM);

                // if the direction is wrong, correct first
                if(abs(va) < maxRotationPWM) {
                    bb::chalkbotMotorController.setVelocitySmooth(vx, va);
                    // only print, when driving
                    bb::printDriver.setSpeed(request.p_pwm);
                } else {
                    bb::chalkbotMotorController.setVelocitySmooth(0, va);
                }

                bb::behavior.moving = true;
            }
            else
            {
                stop = true;
            }

            auto writer = logger.writer_debug();
            writer.print("GOTO: ");
            writer.print(targetPointLocal.x);
            writer.print(",");
            writer.print(targetPointLocal.y);
            writer.print(",");
            writer.print(request.p_pwm);

            break;
        }


        default:
            stop = true;
    }

    if (stop)
    {
        bb::chalkbotMotorController.setVelocitySmooth(0, 0);
        bb::printDriver.setSpeed(0);
        bb::behavior.moving = false;
    }
}
