#ifndef IMU_BB_H
#define IMU_BB_H

#include <Arduino.h>

class ImuBB {
    friend class Imu;

    bool initialized = false;

    // X = right, Y = forward, Z = up
    double heading = 0.0; // deg,    0 to 360, Z counterclockwise / Y compass direction
    double roll = 0.0;    // deg,  -90 to  90, Y counterclockwise / X inclination
    double pitch = 0.0;   // deg, -180 to 180, X counterclockwise / Y declension

    // counterclockwise rotation in 2D space in rad, -pi to +pi (opposite direction of heading)
    double orientation = 0.0;

    // 0 = not calibrated, 3 = fully calibrated
    uint8_t systemCalibration = 0;
    uint8_t gyroscopeCalibration = 0;
    uint8_t accelerometerCalibration = 0;
    uint8_t magnetometerCalibration = 0;

public:
    bool isInitialized() const {
        return initialized;
    }

    double getHeading() const {
        return heading;
    }

    double getRoll() const {
        return roll;
    }

    double getPitch() const {
        return pitch;
    }


    double getOrientation() const {
        return orientation;
    }

    int getSystemCalibration() const {
        return systemCalibration;
    }

    int getGyroscopeCalibration() const {
        return gyroscopeCalibration;
    }

    int getAccelerometerCalibration() const {
        return accelerometerCalibration;
    }

    int getMagnetometerCalibration() const {
        return magnetometerCalibration;
    }
};

#endif
