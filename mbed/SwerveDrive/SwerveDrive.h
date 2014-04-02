#ifndef _SWERVEDRIVE_H_
#define _SWERVEDRIVE_H_
#include "defines.h"
#include "mbed.h"
#include "rtos.h"
#include "MotorDriver.h"
#include "IMU.h"

const int kModuleControlPeriodMs = 10;
const int kModuleRadiusCm = 20;
const int kModuleTheta[3] = {0, 120, -120};  // location of module relative to base

class SwerveDrive {
	public:
  	SwerveDrive(MotorDriver m0_steer, MotorDriver m0_drive,
                MotorDriver m1_steer, MotorDriver m1_drive,
                MotorDriver m2_steer, MotorDriver m2_drive);
    void set_setpoints(int t_mag_setp_world, int t_head_setp_world, int rot_setp_world);

    int m0_rot_setp;
    int m0_vel_setp;
    int m1_rot_setp;
    int m1_vel_setp;
    int m2_rot_setp;
    int m2_vel_setp;

  protected:
    MotorDriver _m0_steer;
    MotorDriver _m0_drive;
    MotorDriver _m1_steer;
    MotorDriver _m1_drive;
    MotorDriver _m2_steer;
    MotorDriver _m2_drive;

    RtosTimer _module_control_timer;

    int _t_mag_setp_world;
    int _t_head_setp_world;
    int _rot_setp_world;

    static void _module_control_static_callback(void const *args);
    void _module_control();
    void _calculate_module_setp(int m_idx, int *m_rot_setp, int *m_vel_setp);
};

#endif // _SWERVEDRIVE_H_
