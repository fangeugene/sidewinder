#ifndef _IMU_H_
#define _IMU_H_
#include "defines.h"
#include "mbed.h"
#include "rtos.h"
#include "ITG3200.h"
#include "MAG3110.h"

const int kGyroCalibCycles = 1000;
const int kGyroIntegrationMs = 1;

class IMU {
  public:
    IMU(PinName gyro_sda, PinName gyro_scl, PinName mag_sda, PinName mag_scl);
    int get_angle();

  // protected:
    // ITG3200 _gyro;
    MAG3110 _mag;

    int _minX;
    int _maxX;
    int _minY;
    int _maxY;

//     int _gyro_offset;
//     int _gyro_angle;

//     void _gyro_calibrate();
//     void _gyro_integrate(void const *args);
};

#endif // _IMU_H_
