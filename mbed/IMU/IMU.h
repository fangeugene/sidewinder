#ifndef _IMU_H_
#define _IMU_H_
#include "mbed.h"
#include "rtos.h"
#include "ITG3200.h"

namespace imu {
  void gyro_calibrate();
  void gyro_integrate(void const *args);
  int get_angle();

  const int kGyroCalibCycles = 1000;
  const int kGyroIntegrationMs = 1;
}
  
#endif // _IMU_H_
