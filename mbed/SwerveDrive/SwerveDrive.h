#ifndef _SWERVEDRIVE_H_
#define _SWERVEDRIVE_H_
#include "defines.h"
#include "mbed.h"
#include "rtos.h"
#include "IMU.h"

namespace swervedrive {
  void init();
  void set_setpoints(int t_mag_setp, int t_head_setp, int rot_setp);
  void module_control(void const *args);
  void calculate_module_setpoints(int module_idx, float module_r, float module_theta, float t_mag_setp, float t_head_setp, float rot_setp);
  int read_r(int module_idx);
  int read_theta(int module_idx);
  
  const int kModuleControlPeriodMs = 10;
  const int kModuleRadiusCm = 20;
}
  
#endif // _SWERVEDRIVE_H_
