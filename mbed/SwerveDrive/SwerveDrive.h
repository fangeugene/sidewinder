#ifndef _SWERVEDRIVE_H_
#define _SWERVEDRIVE_H_
#include "defines.h"
#include "mbed.h"
#include "rtos.h"
#include "QEI.h"
#include "MotorDriver.h"
#include "IMU.h"

const int kWatchdogTimeoutMs = 250;

const int kModuleControlPeriodMs = 10;
const int kModuleRadiusCm = 20;
const int kModuleTheta[3] = {0, 120, -120};  // location of module relative to base
const PinName kNCPin = PTA17;
const int kDriveEncoderCPR = 48;
const int kSteerEncoderCPR = 192;

const int kPSteering = 200;
const int kDSteering = 150;

const float kPHeading = 0.2;
const float kDHeading = 3.0;

// Modes
typedef enum MODES {
  MODE_TELEOP_WC,
	MODE_TELEOP_RC,
  MODE_TRACK
} Mode;


class SwerveDrive {
  public:
    SwerveDrive(IMU* imu,
                MotorDriver m0_steer, MotorDriver m0_drive,
                MotorDriver m1_steer, MotorDriver m1_drive,
                MotorDriver m2_steer, MotorDriver m2_drive);
    void enable();
    void disable();
    void feed_watchdog();
	
	  void set_mode_teleop_wc();
	  void set_mode_teleop_rc();
	  void set_mode_track();
	
    void set_setpoints1(int t_mag_setp_world, int t_head_setp_world, int rot_vel);
    void update_rot_setp_world(int rot_setp_diff);

    int m0_rot_setp;
    int m0_vel_setp;
    int m1_rot_setp;
    int m1_vel_setp;
    int m2_rot_setp;
    int m2_vel_setp;

    int angle;

  // protected:
    IMU* _imu;
    MotorDriver _m0_steer;
    MotorDriver _m0_drive;
    MotorDriver _m1_steer;
    MotorDriver _m1_drive;
    MotorDriver _m2_steer;
    MotorDriver _m2_drive;

    QEI _m0_steer_encoder;
    QEI _m0_drive_encoder;
    QEI _m1_steer_encoder;
    QEI _m1_drive_encoder;
    QEI _m2_steer_encoder;
    QEI _m2_drive_encoder;

    RtosTimer _module_control_timer;
    RtosTimer _watchdog_timer;

    bool _enabled;
		
		Mode _mode;

    int _t_mag_setp_world;
    int _t_head_setp_world;
    int _rot_vel;

    int _rot_setp_world;

    int _last_heading_error;

    int _last_m0_steer_error;
    int _last_m1_steer_error;
    int _last_m2_steer_error;

    static void _module_control_static_callback(void const *args);
    static void _watchdog_static_callback(void const *args);
    void _module_control();
    void _calculate_module_setp(int m_idx, int m_angle, int *m_rot_setp, int *m_vel_setp);
};

#endif // _SWERVEDRIVE_H_
