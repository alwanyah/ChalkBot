#include "PoseFusion.h"
#include "../BB.h"
#include "../Config.h"

// TODO: use filter

void PoseFusion::update() 
{
    const Pose2D currentOdometry = bb::odometry.getRobotPose();
    const Pose2D odomertyDelta = currentOdometry - lastOdometry;

    updateByOdometry(odomertyDelta);
    
    const unsigned long currentGnssTimestamp = bb::gnss.getRelativeTimestamp();
    if (currentGnssTimestamp != lastGnssTimestamp) 
    {
        resetWeights();
      
        const double gnssNorth =  bb::gnss.getNorth() * 1000.0;
        const double gnssEast  = -bb::gnss.getEast()  * 1000.0;
        
        Vector2d gps(gnssNorth, gnssEast);
        
        if(!bb::poseFusion.useGnss) {
          gps_origin = gps;
        } else {
          gps -= gps_origin;
        }
        
        calculateWeightsByGPS(gps);
        
        simpleResample(); 
        
        //const double gnssNorthAccuracy = bb::gnss.getNorthAccuracy();
        //const double gnssEastAccuracy = bb::gnss.getEastAccuracy();

        //bb::poseFusion.fusedPose.translation.x = poseNorth;
        //bb::poseFusion.fusedPose.translation.y = poseEast;

        lastGnssTimestamp = currentGnssTimestamp;
    }
    
    //calculateCurrentPose();
    
    if(!bb::poseFusion.useGnss) {
      init(bb::poseFusion.fusedPose);
    }
    
    calculateMeanCurrentPose();

    lastOdometry = currentOdometry;
}
