#include "Behavior.h"
#include "BB.h"
#include "ChalkbotMotor.h"
#include "Printer.h"
#include "Logger.h"

BehaviorClass Behavior;

static Logger logger("behavior");

// FIXME: Convert all Serial.print to logger.log



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
    ChalkbotMotor.setVelocitySmooth(x, va);

    // prioritize turn correction
    /*
    if(abs(va) < maxRotationPWM) {
        ChalkbotMotor.setVelocitySmooth(x, va);
    } else {
        ChalkbotMotor.setVelocitySmooth(0, va);
    }
    */

    // print
    Printer.setSpeed(printSpeed);
}

void BehaviorClass::update() {
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

            ChalkbotMotor.setMotorVelocity(
                request.motorFrontLeftSpeed,
                request.motorFrontRightSpeed,
                request.motorRearLeftSpeed,
                request.motorRearRightSpeed
            );
            Printer.setSpeed(request.printerSpeed);

            bb::behavior.moving = true;
            break;
        }

        case Command::DRIVE: {
            auto request = bb::webServer.getDriveRequest();
            if (elapsed >= request.duration) {
                stop = true;
                break;
            }

            // FIXME: sanity check parameters (conversion int16_t -> float)
            ChalkbotMotor.setVelocitySmooth(request.x_pwm, request.a_pwm);
            Printer.setSpeed(request.p_pwm);

            Serial.println("DRIVE");
            bb::behavior.moving = true;
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

            // FIXME: logger
            // if(chalkWebServer.debug == 1) {
            //     Serial.print("DRIVE_IMU: ");
            //     Serial.print(chalkWebServer.driveIMURequest.vx);
            //     Serial.print(", ");
            //     Serial.print(chalkWebServer.driveIMURequest.va);
            //     Serial.print(", ");
            //     Serial.print(chalkWebServer.driveIMURequest.p);
            //     Serial.print(", ");
            //     Serial.print(chalkWebServer.driveIMURequest.duration);
            //     Serial.println("");
            // }

            bb::behavior.moving = true;
        }

        case Command::GOTO: {
            auto request = bb::webServer.getGotoPointRequest();

            // FIXME: move to Config.h
            float maxRotationPWM = 64.0f;

            Pose2D robotPose = bb::odometry.getRobotPose();

            // transform the target from global coordinates into local coordinates of the robot
            Vector2d targetPointLocal = robotPose / request.target;

            // constraint max velocity
            float vx = constrain(targetPointLocal.abs(), -maxRotationPWM, maxRotationPWM);

            float maxDistanceError = 30;

            Serial.println(vx);
            if(abs(vx) > maxDistanceError)
            {
                // P-controller
                float va = (float)(targetPointLocal.angle() * 1024.0f);
                va = constrain(va, -maxRotationPWM, maxRotationPWM);

                // if the direction is wrong, correct first
                if(abs(va) < maxRotationPWM) {
                    ChalkbotMotor.setVelocitySmooth(vx, va);
                    // only print, when driving
                    Printer.setSpeed(request.p_pwm);
                } else {
                    ChalkbotMotor.setVelocitySmooth(0, va);
                }

                bb::behavior.moving = true;
            }
            else
            {
                stop = true;
            }

            // FIXME: logger
            // if(chalkWebServer.debug == 10) {
            //     Serial.print(targetPointLocal.x);
            //     Serial.print(",");
            //     Serial.print(targetPointLocal.y);
            //     //Serial.print(",");
            //     //Serial.print(chalkWebServer.gotoPointRequest.p_pwm);
            //     Serial.println("");
            // }
        }


        default:
            stop = true;
    }

    if (stop)
    {
        ChalkbotMotor.setVelocitySmooth(0, 0);
        Printer.setSpeed(0);
        bb::behavior.moving = false;
    }
}
