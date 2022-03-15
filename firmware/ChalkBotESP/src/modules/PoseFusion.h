#ifndef POSE_FUSION_H
#define POSE_FUSION_H

#include <Arduino.h>
#include "../util/math/Pose2D.h"

#include "../BB.h"
#include "../Config.h"

#include <vector>

class PoseFusion 
{
private:
    Pose2D lastOdometry;
    unsigned long lastGnssTimestamp = 0;

    std::vector<Pose2D> samples;
    std::vector<double> weights;

public:
    
    PoseFusion()
    {
      samples.resize(10);
      weights.resize(10);
      
      resetWeights();
      applySimpleNoise(300, Math::pi);
    }
    
    void resetWeights() {
      const double likelihood = 1.0 / static_cast<double>(samples.size());
      for(double& w: weights) {
        w = likelihood;
      }
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
        applySimpleNoise(s, distanceNoise, angleNoise);
      }
    }
    
    void applySimpleNoise(Pose2D& s, double distanceNoise, double angleNoise)
    {
      s.translation.x += (Math::random()-0.5)*distanceNoise;
      s.translation.y += (Math::random()-0.5)*distanceNoise;
      s.rotation = Math::normalize(s.rotation + (Math::random()-0.5)*angleNoise);
    }
    
    void calculateWeightsByGPS(const Vector2d& gps) 
    {
      for(int i = 0; i < samples.size(); ++i) {
        weights[i] = -(samples[i].translation - gps).abs2();
      }
    }
    
    void simpleResample() 
    {
      // find min/max
      int max_i = 0;
      double max_w = weights[max_i];
      int min_i = 0;
      double min_w = weights[min_i];
      for(int i = 0; i < weights.size(); ++i) {
        if(max_w < weights[i]) {
          max_i = i;
          max_w = weights[i];
        } else if(min_w > weights[i]) {
          min_i = i;
          min_w = weights[i];
        }
      }
      
      // replace the worst with a copy of the best
      samples[min_i] = samples[max_i];
      applySimpleNoise(samples[min_i], 100, Math::pi / 180.0);
    }
    
    void calculateMaxCurrentPose() 
    {
      int max_i = 0;
      double max_w = weights[max_i];
      for(int i = 0; i < weights.size(); ++i) {
        if(max_w < weights[i]) {
          max_i = i;
          max_w = weights[i];
        }
      }
      
      bb::poseFusion.fusedPose = samples[max_i];
    }
    
    void calculateMeanCurrentPose()
    {
      Vector2d averageTranslation;
      Vector2d averageRotation;

      for(Pose2D& s: samples)
      {
        averageTranslation += s.translation;
        averageRotation.x += cos(s.rotation);
        averageRotation.y += sin(s.rotation);
      }

      const double n = static_cast<double>(samples.size());
      bb::poseFusion.fusedPose.translation = averageTranslation / n;
      bb::poseFusion.fusedPose.rotation = (averageRotation / n).angle();
    }

    void update();
    
};

#endif
