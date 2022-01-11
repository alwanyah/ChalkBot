
#ifndef WEB_SERVER_BB_H
#define WEB_SERVER_BB_H

#include <Arduino.h>
#include "math/Vector2.h"

// FIXME: Command vs Request
// FIXME: camelCase vs snake_case

// commands that can be handeled by the server
enum class Command {
  NONE = 0, // FIXME: do we need numbers?
  DRIVE = 1,
  MOTOR_TEST = 2,
  DRIVE_IMU = 3,
  GOTO = 4,
};

struct MotorTestRequest {
  int16_t motorFrontLeftSpeed;
  int16_t motorFrontRightSpeed;
  int16_t motorRearLeftSpeed;
  int16_t motorRearRightSpeed;
  int16_t printerSpeed;
};

struct DriveRequest {
  int16_t x_pwm    = 0; // [-255,255] drive
  int16_t a_pwm    = 0; // [-255,255] turn
  int16_t p_pwm    = 0; // [0,255] print
  int16_t duration = 0; // in ms
};

struct DriveImuRequest {
  int16_t vx       = 0;
  float va         = 0;
  int16_t p        = 0;
  int16_t duration = 0;
};

// target point for the robot to drive to
struct GotoPointRequest {
  Vector2d target;
  int16_t p_pwm = 0;
};

class WebServerBB
{
  friend class WebServerClass;

  Command lastCommand = Command::NONE;

  unsigned long timeOfLastCommand = 0; // ms

  // FIXME: this should probably be an union
  MotorTestRequest motorTestRequest;
  DriveRequest driveRequest;
  DriveImuRequest driveIMURequest;
  GotoPointRequest gotoPointRequest;

public:
  Command getLastCommand() const {
    return lastCommand;
  }

  unsigned long getTimeOfLastCommand() const {
    return timeOfLastCommand;
  }

  const MotorTestRequest &getMotorTestRequest() const {
    return motorTestRequest;
  }

  const DriveRequest &getDriveRequest() const {
    return driveRequest;
  }

  const DriveImuRequest &getDriveIMURequest() const {
    return driveIMURequest;
  }

  const GotoPointRequest &getGotoPointRequest() const {
    return gotoPointRequest;
  }
};

#endif
