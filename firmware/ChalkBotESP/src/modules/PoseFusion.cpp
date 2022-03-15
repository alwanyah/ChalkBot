#include "PoseFusion.h"
#include "../BB.h"
#include "../Config.h"
#include "../util/Logger.h"

static Logger logger("fusion");

// TODO: use filter

void PoseFusion::update() 
{
    const Pose2D currentOdometry = bb::odometry.getRobotPose();
    const Pose2D odomertyDelta = currentOdometry - lastOdometry;

    updateByOdometry(odomertyDelta);
    
    const unsigned long currentGnssTimestamp = bb::gnss.getRelativeTimestamp();
    if (currentGnssTimestamp != lastGnssTimestamp) 
    {
        const double gnssNorth =  bb::gnss.getNorth() * 1000.0;
        const double gnssEast  = -bb::gnss.getEast()  * 1000.0;
        
        Vector2d gps(gnssNorth, gnssEast);
        
        if(!bb::poseFusion.useGnss) 
        {
          gps_origin = gps;
        } 
        else // use gps 
        {
          gps -= gps_origin;

          if(!init_localization) {
            init(bb::poseFusion.fusedPose);
            resetWeights();
            applySimpleNoise(300, Math::pi2);
            init_localization = true;
          }
         
          resetWeights();
          calculateWeightsByGPS(gps);
        
          simpleResample();
        }
        
        //const double gnssNorthAccuracy = bb::gnss.getNorthAccuracy();
        //const double gnssEastAccuracy = bb::gnss.getEastAccuracy();

        //bb::poseFusion.fusedPose.translation.x = poseNorth;
        //bb::poseFusion.fusedPose.translation.y = poseEast;

        lastGnssTimestamp = currentGnssTimestamp;
    }
    
    //calculateCurrentPose();
    
    if(!bb::poseFusion.useGnss) {
      init(bb::poseFusion.fusedPose);
      init_localization = false;
    }
    
    calculateMeanCurrentPose();

    lastOdometry = currentOdometry;
    
    // copy samples to the bb 
    bb::poseFusion.samples = samples;
    
    // log stuff
    auto writer = logger.writer_debug();
    writer.print(bb::poseFusion.fusedPose.translation.x);
    writer.print(", ");
    writer.print(bb::poseFusion.fusedPose.translation.y);
    writer.print(", ");
    writer.print(bb::poseFusion.fusedPose.rotation);
    writer.finish();
}
