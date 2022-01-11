#ifndef ODOMETRY_H
#define ODOMETRY_H

class OdometryClass {
    unsigned long lastUpdateTime = 0;

public:
    void update();
};

extern OdometryClass Odometry;

#endif
