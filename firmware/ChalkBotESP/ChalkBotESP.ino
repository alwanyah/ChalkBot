#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

#include "ChalkbotMotorDriver.h"
#include "PrintDriver.h"

#include "ChalkWebServerAsync.h"

#include "math/Common.h"
#include "math/Vector2.h"
#include "math/Pose2D.h"

// mapping stm32 feather -> esp32 feather
// PWM 5-> 4; 6-> 18; 9-> 19; 10-> 16
// DIR A0->A12 ;A1->A11 ;A2->A10 ;A3->A9  
//PRINT: 23-> 5 (SCK); A4 -> A8; A5->A7

// Configure the motor driver.
ChalkbotMD chalkbot(
  { A7, A8 }, // motorFrontLeft : PWM =  4,  DIR = A12
  { 16, 17 }, // -motorFrontRight: PWM =  18, DIR = A11
  { A12,A11}, // motorRearLeft  : PWM =  19, DIR = A10
  { A0, A1 }  // motorRearRight : PWM =  16, DIR = A9
); 

// GPIO23 = SCK
PrintMD printer(21, A5, A6); // 

ChalkWebServerAsync chalkWebServer;

Adafruit_BNO055 bno = Adafruit_BNO055(55);
double orientation = 0.0;

// time of the last execution
unsigned long currentTime = 0;
// time since that last execution cycle
unsigned long timeSenceLastExecution = 0;

// odometry based robot pose
Pose2D robotPose;

// The setup routine runs once when you press reset.
void setup() 
{
  Serial.begin(115200);
  while (!Serial) {};

  chalkWebServer.initAP("chalkbot", "a1b0a1b0a1");
  //chalkWebServer.init("LAMBARUM", "DBiru1Sd90M.");
  //chalkWebServer.init("NAONET", "a1b0a1b0a1");
  
  Serial.print("AP IP address: ");
  Serial.println(chalkWebServer.gerServerIP());

  if(!bno.begin()) {
    Serial.println("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    chalkWebServer.status_imu = "failed to connect";
  } else {
    Serial.println("BNO055 connected!");
    delay(1000);
    bno.setExtCrystalUse(true);
    chalkWebServer.status_imu = "ok";
  }
  delay(1000);

  // test functions
  //float t = distanceToTime(3200, 64);
  //float d = timeToDistance(t, 64);
  //Serial.println(d);
}

// controller for closed loop rotation control
void driveWithTimeIMU(float x, float angle, int printSpeed)
{
  // diff angle to turn
  double diff = Math::normalize(angle - orientation);

  // P-controller
  float va = (float)(diff*1024.0f);

  // constraint max velocity
  float maxRotationPWM = 64.0f;
  va = constrain(va, -maxRotationPWM, maxRotationPWM);

  if(chalkWebServer.debug == 2) {
    Serial.print(va);
  }

  // continuous correction
  chalkbot.setVelocitySmooth(x, va);
  
  // prioritize turn correction
  /*
  if(abs(va) < maxRotationPWM) { 
    chalkbot.setVelocitySmooth(x, va);
  } else {
    chalkbot.setVelocitySmooth(0, va);
  }
  */

  // print
  printer.setSpeed(printSpeed);
}


// fixed parameters
static const float wheelDiameter = 135.0f; // mm  
static const float maxRPM = 313.0f; // max RPM of the motors with PWM 255
static const float wheelUmfang = wheelDiameter*PI;
static const float maxVelocity = wheelUmfang * maxRPM / 60.0f; // mm/s

//measured factor for the current chalkbot
static const float correctionFactor = 4.0 / 3.0;

// Experimental - not used yet
// distance - target distance to drive
// pwm - target pwm to be used
float distanceToTime(float distance, float pwm) 
{
  float targetVelocity = maxVelocity * (pwm / 255.0f);
  float timeNeeded = distance / targetVelocity; // in s

  timeNeeded *= correctionFactor;
  
  return timeNeeded; 
}

// estimate the distance that the robot moved in a given time
float timeToDistance(float timeDelta, float pwm) 
{
  float targetVelocity = maxVelocity * (pwm / 255.0f);
  float distance = timeDelta * targetVelocity;

  distance /= correctionFactor;

  return distance; 
}

void updateOdometry() 
{
  // the robot was driving with the past motion commands since the last frame.
  // update the odometry based on that information
  int16_t last_x_PWM = chalkbot.getLastX();
  //int16_t last_rotation_PWM = chalkbot.getLastRotation();

  // use directly the direction from the IMU
  robotPose.rotation = chalkWebServer.orientation;

  float timeDelta = float(timeSenceLastExecution) / 1000.0f; // in s
  float distanceMoved = timeToDistance(timeDelta, last_x_PWM);
  robotPose.translate(distanceMoved, 0);
}

void loop() 
{
  // update orientation
  imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
  orientation = Math::fromDegrees(-euler.x());
  orientation = Math::normalize(orientation);
  // communicate to the server
  chalkWebServer.orientation = orientation;

  // update time
  unsigned long lastUpdateTime = currentTime;
  currentTime = millis();
  timeSenceLastExecution = currentTime - lastUpdateTime;

  // update the odometry by the movement sonce the last command
  updateOdometry();
  chalkWebServer.robotPose = robotPose;

  if(chalkWebServer.debug == 3) {
	  Serial.print(robotPose.translation.x);
	  Serial.print(", ");
	  Serial.print(robotPose.translation.y);
	  Serial.print(", ");
	  Serial.println(robotPose.rotation);
  }

  // execute commands
  if(chalkWebServer.lastCommand == ChalkWebServerAsync::MOTOR_TEST && currentTime < chalkWebServer.timeOfLastCommand + 3000) 
  {
    chalkbot.setMotorVelocity(
      chalkWebServer.motor_speed[0],
      chalkWebServer.motor_speed[1],
      chalkWebServer.motor_speed[2],
      chalkWebServer.motor_speed[3]
    );
    printer.setSpeed(chalkWebServer.motor_speed[4]);

    chalkWebServer.status_motion = "moving";
  } 
  else if(chalkWebServer.lastCommand == ChalkWebServerAsync::DRIVE && currentTime < chalkWebServer.timeOfLastCommand + chalkWebServer.driveRequest.duration) 
  {
    chalkbot.setVelocitySmooth(chalkWebServer.driveRequest.x_pwm, chalkWebServer.driveRequest.a_pwm);
    printer.setSpeed(chalkWebServer.driveRequest.p_pwm);

    Serial.println("DRIVE");
    chalkWebServer.status_motion = "moving";
  } 
  else if(chalkWebServer.lastCommand == ChalkWebServerAsync::DRIVE_IMU && currentTime < chalkWebServer.timeOfLastCommand + chalkWebServer.driveIMURequest.duration)
  {
    driveWithTimeIMU(
      chalkWebServer.driveIMURequest.vx,  
      chalkWebServer.driveIMURequest.va,  
      chalkWebServer.driveIMURequest.p);

    if(chalkWebServer.debug == 1) {
      Serial.print("DRIVE_IMU: ");
      Serial.print(chalkWebServer.driveIMURequest.vx);
      Serial.print(", ");
      Serial.print(chalkWebServer.driveIMURequest.va);
      Serial.print(", ");
      Serial.print(chalkWebServer.driveIMURequest.p);
      Serial.print(", ");
      Serial.print(chalkWebServer.driveIMURequest.duration);
      Serial.println("");
    }
    
    chalkWebServer.status_motion = "moving";
  }
  else if(chalkWebServer.lastCommand == ChalkWebServerAsync::GOTO)
  {
    float maxRotationPWM = 64.0f;

    // transform the target from global coordinates into local coordinates of the robot
    Vector2d targetPointLocal = robotPose / chalkWebServer.gotoPointRequest.target;

    // constraint max velocity
    float vx = constrain(targetPointLocal.abs(), -maxRotationPWM, maxRotationPWM);
    int16_t p_pwm = chalkWebServer.gotoPointRequest.p_pwm;

    float maxDistanceError = 30;

    Serial.println(vx);
    if(abs(vx) > maxDistanceError) 
    {
      // P-controller
      float va = (float)(targetPointLocal.angle() * 1024.0f);
      va = constrain(va, -maxRotationPWM, maxRotationPWM);
  
      // if the direction is wrong, correct first
      if(abs(va) < maxRotationPWM) {
        chalkbot.setVelocitySmooth(vx, va);
        // only print, when driving
        printer.setSpeed(p_pwm);
      } else {
        chalkbot.setVelocitySmooth(0, va);
      }

      chalkWebServer.status_motion = "moving";
    }
    else
    {
      chalkbot.setVelocitySmooth(0, 0);
      printer.setSpeed(0);
      chalkWebServer.status_motion = "stopped";
    }

    if(chalkWebServer.debug == 10) {
      Serial.print(targetPointLocal.x);
      Serial.print(",");
      Serial.print(targetPointLocal.y);
      //Serial.print(",");
      //Serial.print(chalkWebServer.gotoPointRequest.p_pwm);
      Serial.println("");
    }
  }
  else // default: stop
  {
    chalkbot.setVelocitySmooth(0, 0);
    printer.setSpeed(0);
    chalkWebServer.status_motion = "stopped";
  }
}
