#ifndef POSE_FUSION_H
#define POSE_FUSION_H

#include <Arduino.h>
#include "../util/math/Pose2D.h"

#include <vector>

class PoseFusion 
{
private:
    Pose2D lastOdometry;
    unsigned long lastGnssTimestamp = 0;

    std::vector<Pose2D> samples;
    std::vector<double> weights;

public:
    
    PoseFusion() {
      samples.resize(10);
      weights.resize(10);
      
      applySimpleNoise(300, Math::pi);
    }
    
    void init(const Pose2D& pose) {
      for(Pose2D& s: samples) {
        s = pose;
      }
    }
    
    void updateByOdometry(const Pose2D& odomertyDelta) {
      for(Pose2D& s: samples) {
        s += odomertyDelta;
      }
    }
    
    void applySimpleNoise(double distanceNoise, double angleNoise)
    {
      for(Pose2D& s: samples) {
        s.translation.x += (Math::random()-0.5)*distanceNoise;
        s.translation.y += (Math::random()-0.5)*distanceNoise;
        s.rotation = Math::normalize(s.rotation + (Math::random()-0.5)*angleNoise);
      }
    }

    void update();
};

#endif
