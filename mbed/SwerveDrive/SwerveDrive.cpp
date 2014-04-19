#include "SwerveDrive.h"

SwerveDrive::SwerveDrive(IMU* imu,
                         MotorDriver m0_steer, MotorDriver m0_drive,
                         MotorDriver m1_steer, MotorDriver m1_drive,
                         MotorDriver m2_steer, MotorDriver m2_drive):
                         _m0_steer(m0_steer), _m0_drive(m0_drive),
                         _m1_steer(m1_steer), _m1_drive(m1_drive),
                         _m2_steer(m2_steer), _m2_drive(m2_drive),
                         _m0_steer_encoder(PTC7, PTA5, kNCPin, kSteerEncoderCPR, QEI::X4_ENCODING), _m0_drive_encoder(PTC6, PTA4, kNCPin, kDriveEncoderCPR, QEI::X4_ENCODING),
                         _m1_steer_encoder(PTC4, PTA12, kNCPin, kSteerEncoderCPR, QEI::X4_ENCODING), _m1_drive_encoder(PTC0, PTD3, kNCPin, kDriveEncoderCPR, QEI::X4_ENCODING),
                         _m2_steer_encoder(PTC11, PTC9, kNCPin, kSteerEncoderCPR, QEI::X4_ENCODING), _m2_drive_encoder(PTC10, PTC8, kNCPin, kDriveEncoderCPR, QEI::X4_ENCODING),
                         _module_control_timer(SwerveDrive::_module_control_static_callback, osTimerPeriodic, this),
                         _watchdog_timer(SwerveDrive::_watchdog_static_callback, osTimerPeriodic, this) {
  _enabled = false;
  _mode = MODE_TELEOP;

  _t_mag_setp_world = 0;
  _t_head_setp_world = 0;
  _rot_setp_world = 0;

  m0_rot_setp = 0;
  m0_vel_setp = 0;
  m1_rot_setp = 0;
  m1_vel_setp = 0;
  m2_rot_setp = 0;
  m2_vel_setp = 0;

  _last_heading_error = 0;

  _last_m0_steer_error = 0;
  _last_m1_steer_error = 0;
  _last_m2_steer_error = 0;

  _module_control_timer.start(kModuleControlPeriodMs);
  _watchdog_timer.start(kWatchdogTimeoutMs);
}

void SwerveDrive::enable() {
  _enabled = true;
}

void SwerveDrive::disable() {
  _enabled = false;
}

void SwerveDrive::_watchdog_static_callback(void const *args) {
  // Static callback -- see https://mbed.org/forum/mbed/topic/4388/ for more info
  SwerveDrive *instance = (SwerveDrive*)args;
  instance->disable();
}

void SwerveDrive::feed_watchdog() {
  _watchdog_timer.start(kWatchdogTimeoutMs);
}

void SwerveDrive::set_mode_teleop() {
	_mode = MODE_TELEOP;
}

void SwerveDrive::set_mode_track() {
	_mode = MODE_TRACK;
}
void SwerveDrive::set_setpoints1(int t_mag_setp_world, int t_head_setp_world, int rot_vel) {
  _t_mag_setp_world = t_mag_setp_world;
  _t_head_setp_world = t_head_setp_world;
  _rot_vel = rot_vel;
}

void SwerveDrive::update_rot_setp_world(int rot_setp_diff) {
	if (_mode == MODE_TRACK) {
  	_rot_setp_world += rot_setp_diff;
	}
}

void SwerveDrive::_module_control_static_callback(void const *args) {
  // Static callback -- see https://mbed.org/forum/mbed/topic/4388/ for more info
  SwerveDrive *instance = (SwerveDrive*)args;
  instance->_module_control();
}

void SwerveDrive::_module_control() {
  if (!_enabled) {
    _m0_drive.setPower(0);
    _m1_drive.setPower(0);
    _m2_drive.setPower(0);
    _m0_steer.setPower(0);
    _m1_steer.setPower(0);
    _m2_steer.setPower(0);
    return;
  }

  // 16/3 clicks per degree
  int m0_angle = _m0_steer_encoder.getPulses() * 3 / 16;
  int m1_angle = _m1_steer_encoder.getPulses() * 3 / 16;
  int m2_angle = _m2_steer_encoder.getPulses() * 3 / 16;

  SwerveDrive::_calculate_module_setp(0, m0_angle, &m0_rot_setp, &m0_vel_setp);
  SwerveDrive::_calculate_module_setp(1, m1_angle, &m1_rot_setp, &m1_vel_setp);
  SwerveDrive::_calculate_module_setp(2, m2_angle, &m2_rot_setp, &m2_vel_setp);

  _m0_drive.setPower(m0_vel_setp);
  _m1_drive.setPower(m1_vel_setp);
  _m2_drive.setPower(m2_vel_setp);

  int m0_steer_error = m0_rot_setp - m0_angle;
  int m1_steer_error = m1_rot_setp - m1_angle;
  int m2_steer_error = m2_rot_setp - m2_angle;

  _m0_steer.setPower(kPSteering * m0_steer_error / 10 - kDSteering * _last_m0_steer_error / 10);
  _m1_steer.setPower(kPSteering * m1_steer_error / 10 - kDSteering * _last_m1_steer_error / 10);
  _m2_steer.setPower(kPSteering * m2_steer_error / 10 - kDSteering * _last_m2_steer_error / 10);

  _last_m0_steer_error = m0_steer_error;
  _last_m1_steer_error = m1_steer_error;
  _last_m2_steer_error = m2_steer_error;
}

void SwerveDrive::_calculate_module_setp(int m_idx, int m_angle, int *m_rot_setp, int *m_vel_setp) {
  // TODO: use integer arithmetic
  float t_head_setp_robot = float(_t_head_setp_world) - float(angle);  // convert from world frame to robot frame

	if (_mode == MODE_TELEOP) {
		if (_rot_vel != 0) {  // Hold angle when no rot vel joystick input
			_rot_setp_world = angle;
		}
	}

	// Controller for rotational velocity
  int heading_error = _rot_setp_world - angle;
  float rot_vel = kPHeading * float(heading_error) - kDHeading * float(_last_heading_error - heading_error);
  rot_vel = constrain(rot_vel, float(-2), float(2));  // So that rotation does not overpower translation

	if (_mode == MODE_TELEOP) {
		rot_vel += 0.05 * float(_rot_vel);  // Add in rot vel joystisck input
  }
	
  _last_heading_error = heading_error;

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

  int vel_setp = int(sqrt(pow(xsum, 2) + pow(ysum, 2)));
  int rot_setp = int(atan(ysum / xsum) * 180 / 3.14159);
  if (xsum < 0) {  // Fixing atan range
    rot_setp += 180;
  } else if (rot_setp >= 180) {
    rot_setp -= 360;
  }

  *m_rot_setp = closer_half_ang(rot_setp, m_angle);
  if (mod(rot_setp, 360) == mod(*m_rot_setp, 360)) {
    *m_vel_setp = vel_setp;
  } else {
    *m_vel_setp = -vel_setp;
  }

}
