#ifndef _IMU_H_
#define _IMU_H_
#include "defines.h"
#include "mbed.h"
#include "rtos.h"
#include "ITG3200.h"
// #include "MAG3110.h"

const int kGyroCalibCycles = 1000;
const int kGyroIntegrationMs = 1;

class IMU {
  public:
    IMU(PinName gyro_sda, PinName gyro_scl, PinName mag_sda, PinName mag_scl);
    int get_angle();
	  void reset();

  // protected:
    ITG3200 _gyro;
    // MAG3110 _mag;

    RtosTimer _gyro_integration_timer;
    Semaphore ts;

    int _gyro_offset;
    int _gyro_angle;

    void _gyro_calibrate();
    static void _gyro_integration_static_callback(void const *args);
    void _gyro_integrate();
};

#endif // _IMU_H_
