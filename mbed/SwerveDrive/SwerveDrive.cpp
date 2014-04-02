#include "SwerveDrive.h"

SwerveDrive::SwerveDrive(MotorDriver m0_steer, MotorDriver m0_drive,
                         MotorDriver m1_steer, MotorDriver m1_drive,
                         MotorDriver m2_steer, MotorDriver m2_drive):
                         _m0_steer(m0_steer), _m0_drive(m0_drive),
                         _m1_steer(m1_steer), _m1_drive(m1_drive),
                         _m2_steer(m2_steer), _m2_drive(m2_drive),
                         _module_control_timer(SwerveDrive::_module_control_static_callback, osTimerPeriodic, this) {
  _t_mag_setp_world = 0;
  _t_head_setp_world = 0;
  _rot_setp_world = 0;

  m0_rot_setp = 0;
  m0_vel_setp = 0;
  m1_rot_setp = 0;
  m1_vel_setp = 0;
  m2_rot_setp = 0;
  m2_vel_setp = 0;

  _module_control_timer.start(kModuleControlPeriodMs);
}

void SwerveDrive::set_setpoints(int t_mag_setp_world, int t_head_setp_world, int rot_setp_world) {
  _t_mag_setp_world = t_mag_setp_world;
  _t_head_setp_world = t_head_setp_world;
  _rot_setp_world = rot_setp_world;
}

void SwerveDrive::_module_control_static_callback(void const *args) {
  // Static callback -- see https://mbed.org/forum/mbed/topic/4388/ for more info
  SwerveDrive *instance = (SwerveDrive*)args;
  instance->_module_control();
}

void SwerveDrive::_module_control() {
  SwerveDrive::_calculate_module_setp(0, &m0_rot_setp, &m0_vel_setp);
  SwerveDrive::_calculate_module_setp(1, &m1_rot_setp, &m1_vel_setp);
  SwerveDrive::_calculate_module_setp(2, &m2_rot_setp, &m2_vel_setp);
}

void SwerveDrive::_calculate_module_setp(int m_idx, int *m_rot_setp, int *m_vel_setp) {
  // TODO: use integer arithmetic
  float t_head_setp_robot = float(_t_head_setp_world) - float(imu::get_angle());  // convert from world frame to robot frame

  // TODO: temp P controller for rotational velocity
  float rot_vel = 0.2 * (_rot_setp_world - float(imu::get_angle()));

  float rot_comp_mag = kModuleRadiusCm * rot_vel;
  float rot_comp_ang = (kModuleTheta[m_idx] + 90) * 3.14159 / 180;

  float t_comp_mag = _t_mag_setp_world;
  float t_comp_ang = t_head_setp_robot * 3.14159 / 180;

  float x1 = rot_comp_mag * cos(rot_comp_ang);
  float y1 = rot_comp_mag * sin(rot_comp_ang);
  float x2 = t_comp_mag * cos(t_comp_ang);
  float y2 = t_comp_mag * sin(t_comp_ang);

  float xsum = x1 + x2;
  float ysum = y1 + y2;

  *m_vel_setp = int(sqrt(pow(xsum, 2) + pow(ysum, 2)));
  *m_rot_setp = int(atan(ysum / xsum) * 180 / 3.14159);
  if (xsum < 0) {
    *m_rot_setp += 180;
  } else if (*m_rot_setp >= 180) {
    *m_rot_setp -= 360;
  }
}
