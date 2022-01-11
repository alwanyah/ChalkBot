#include "Imu.h"
#include "Logger.h"
#include "BB.h"

#include "math/Common.h"

ImuClass Imu;

static Logger logger("IMU");

bool ImuClass::begin() {
    logger.log_info("Initializing IMU...");
    if(!bno.begin()) {
        logger.log_info("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
        return false;
    } else {
        logger.log_info("BNO055 connected!");
        delay(1000); // FIXME: ist das nötig?
        bno.setExtCrystalUse(true);
        return true;
    }
}

void ImuClass::update() {
    // update orientation
    imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
    double orientation = Math::fromDegrees(-euler.x());
    bb::imu.orientation = Math::normalize(orientation);
}
