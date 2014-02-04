#include "SwerveDrive.h"

namespace swervedrive {
  int g_t_mag_setp;
  int g_t_head_setp;
  int g_rot_setp;
  int g_r[3];
  int g_theta[3];

  void init() {
    g_t_mag_setp = 0;
    g_t_head_setp = 0;
    g_rot_setp = 0;
  }

  void set_setpoints(int t_mag_setp, int t_head_setp, int rot_setp) {
    g_t_mag_setp = t_mag_setp;
    g_t_head_setp = t_head_setp;
    g_rot_setp = rot_setp;
  }
  
  void module_control(void const *args) {
    calculate_module_setpoints(0, kModuleRadiusCm, 0, g_t_mag_setp, g_t_head_setp, g_rot_setp);
    calculate_module_setpoints(1, kModuleRadiusCm, 120, g_t_mag_setp, g_t_head_setp, g_rot_setp);
    calculate_module_setpoints(2, kModuleRadiusCm, -120, g_t_mag_setp, g_t_head_setp, g_rot_setp);
  }
  
  void calculate_module_setpoints(int module_idx, float module_r, float module_theta, float t_mag_setp, float t_head_setp, float rot_setp) {
    t_head_setp -= float(imu::get_angle());

    // temp P controller for rotational velocity
    float rot_vel = 0.2 * (rot_setp - float(imu::get_angle()));

    float rot_comp_mag = module_r * rot_vel;
    float rot_comp_ang = (module_theta + 90) * 3.14159 / 180;

    float t_comp_mag = t_mag_setp;
    float t_comp_ang = t_head_setp * 3.14159 / 180;
    
    float x1 = rot_comp_mag * cos(rot_comp_ang);
    float y1 = rot_comp_mag * sin(rot_comp_ang);
    float x2 = t_comp_mag * cos(t_comp_ang);
    float y2 = t_comp_mag * sin(t_comp_ang);
    
    float xsum = x1 + x2;
    float ysum = y1 + y2;
    
    g_r[module_idx] = int(sqrt(pow(xsum, 2) + pow(ysum, 2)));
    g_theta[module_idx] = int(atan(ysum / xsum) * 180 / 3.14159);
    if (xsum < 0) {
      g_theta[module_idx] += 180;
    }
    if (g_theta[module_idx] >= 180) {
      g_theta[module_idx] -= 360;      
    }
  }
  
  int read_r(int module_idx) {
    return g_r[module_idx];
  }
  
  int read_theta(int module_idx) {
    return g_theta[module_idx];
  }
}
