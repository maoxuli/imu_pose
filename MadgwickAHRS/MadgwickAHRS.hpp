#ifndef MADGWICK_AHRS_H
#define MADGWICK_AHRS_H

#include <Eigen/Eigen>
using namespace Eigen;

class MadgwickAHRS 
{
public:  
    float qw, qx, qy, qz;   // quaternion of sensor frame relative to auxiliary frame

    MadgwickAHRS(float sampleFreq = 512.0f, float beta = 0.1f); 

    void update(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz); 
    void update(float gx, float gy, float gz, float ax, float ay, float az); 

private: 
    float SamplePeriod;     // sample period in seconds
    float Beta;             // algorithm gain
}; 

#endif 