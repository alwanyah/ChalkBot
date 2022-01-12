#include "Odometry.h"
#include "../BB.h"
#include "../Config.h"
#include "../util/Logger.h"

static Logger logger("odometry");

// Experimental - not used yet
// distance - target distance to drive
// pwm - target pwm to be used
static float distanceToTime(float distance, float pwm)
{
  float targetVelocity = config::odometry::maxVelocity * (pwm / 255.0f);
  float timeNeeded = distance / targetVelocity; // in s

  timeNeeded *= config::odometry::correctionFactor;

  return timeNeeded;
}

// estimate the distance that the robot moved in a given time
static float timeToDistance(float timeDelta, float pwm)
{
  float targetVelocity = config::odometry::maxVelocity * (pwm / 255.0f);
  float distance = timeDelta * targetVelocity;

  distance /= config::odometry::correctionFactor;

  return distance;
}

void Odometry::update() {
    // update time
    unsigned long currentTime = millis();
    unsigned long timeSinceLastExecution = currentTime - lastUpdateTime;
    lastUpdateTime = currentTime;

    // update the odometry by the movement since the last command

    // the robot was driving with the past motion commands since the last frame.
    // update the odometry based on that information
    int16_t last_x_PWM = bb::chalkbotMotorController.getCurrentForwardSpeed();
    // int16_t last_rotation_PWM = bb::chalkbotMotorController.getCurrentRotationSpeed();

    // use the direction directly from the IMU
    bb::odometry.robotPose.rotation = bb::imu.getOrientation();

    float timeDelta = float(timeSinceLastExecution) / 1000.0f; // in s
    float distanceMoved = timeToDistance(timeDelta, last_x_PWM);
    bb::odometry.robotPose.translate(distanceMoved, 0);

    auto writer = logger.writer_debug();
    writer.print(bb::odometry.robotPose.translation.x);
    writer.print(", ");
    writer.print(bb::odometry.robotPose.translation.y);
    writer.print(", ");
    writer.print(bb::odometry.robotPose.rotation);
    writer.finish();
}
