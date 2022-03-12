#include "Imu.h"
#include "../BB.h"
#include "../util/Logger.h"
#include "../util/math/Common.h"

static Logger logger("IMU");

bool Imu::begin() {
    logger.log_info("Initializing IMU...");
    if(!bno.begin()) {
        logger.log_info("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
        return false;
    } else {
        logger.log_info("BNO055 connected!");
        delay(1000); // FIXME: ist das nötig?
        bno.setExtCrystalUse(true);
        bb::imu.initialized = true;
        return true;
    }
}

void Imu::update() {
    // update orientation
    imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
    bb::imu.heading = euler.x();
    bb::imu.roll = euler.y();
    bb::imu.pitch = euler.z();

    bb::imu.orientation = Math::normalize(Math::fromDegrees(-euler.x()));

    // update calibration
    bno.getCalibration(
        &bb::imu.systemCalibration,
        &bb::imu.gyroscopeCalibration,
        &bb::imu.accelerometerCalibration,
        &bb::imu.magnetometerCalibration
    );
}
