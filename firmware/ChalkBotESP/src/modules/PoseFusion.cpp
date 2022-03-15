#include "PoseFusion.h"
#include "../BB.h"
#include "../Config.h"

// TODO: use filter

void PoseFusion::update() 
{
    const Pose2D currentOdometry = bb::odometry.getRobotPose();
    const Pose2D odomertyDelta = currentOdometry - lastOdometry;

    //bb::poseFusion.fusedPose += odomertyDelta;
    
    updateByOdometry(odomertyDelta);

    const unsigned long currentGnssTimestamp = bb::gnss.getRelativeTimestamp();
    if (bb::poseFusion.useGnss && currentGnssTimestamp != lastGnssTimestamp) 
    {
        const double gnssNorth =  bb::gnss.getNorth() * 1000.0;
        const double gnssEast  = -bb::gnss.getEast()  * 1000.0;
        
        const double gnssNorthAccuracy = bb::gnss.getNorthAccuracy();
        const double gnssEastAccuracy = bb::gnss.getEastAccuracy();

        double poseNorth = bb::poseFusion.getNorth();
        double poseEast = bb::poseFusion.getEast();

        

        //bb::poseFusion.fusedPose.translation.x = poseNorth;
        //bb::poseFusion.fusedPose.translation.y = poseEast;

        lastGnssTimestamp = currentGnssTimestamp;
    }

    lastOdometry = currentOdometry;
}
