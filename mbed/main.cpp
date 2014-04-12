#include "defines.h"
#include "mbed.h"
#include "rtos.h"
#include "MODSERIAL.h"
#include "BeagleBone.h"
#include "IMU.h"
#include "MotorDriver.h"
#include "SwerveDrive.h"

MODSERIAL pc(USBTX, USBRX);
BeagleBone bone(PTE16, PTE17);

MotorDriver m0_steer(PTB1, PTA6, PTD6); // 2
MotorDriver m0_drive(PTB2, PTA7, PTD4); // 3
MotorDriver m1_steer(PTB3, PTC16, PTD2); // 4
MotorDriver m1_drive(PTC2, PTC13, PTA13);   // 5
MotorDriver m2_steer(PTE31, PTA15, PTD5);  // 0
MotorDriver m2_drive(PTB0, PTA14, PTD7);  // 1

DigitalOut led(LED_RED);

SwerveDrive* swervedrive;
IMU* imu;

void get_msg_thread(void const *args) {
  char msg[kMaxMsgSize];
  while (true) {
    if (bone.has_msg()) {
      bone.read_msg(msg, kMaxMsgSize);
      switch (msg[0]) {
        case '0': {
          if (msg[2] == '1') {
            swervedrive->enable();
          } else {
            swervedrive->disable();
          }
          break;
        }
        case '1': {
          int t_mag_setp = (msg[2] - '0') * 100 + (msg[3] - '0') * 10 + (msg[4] - '0');
          int t_head_setp = (msg[6] - '0') * 100 + (msg[7] - '0') * 10 + (msg[8] - '0');
          int rot_vel_setp = (msg[10] - '0') * 100 + (msg[11] - '0') * 10 + (msg[12] - '0');
          t_head_setp -= 500;
          rot_vel_setp -= 500;
          int rot_setp = 0;
          swervedrive->set_setpoints(t_mag_setp, t_head_setp, rot_vel_setp);
          break;
        }
        case '9': {
          swervedrive->feed_watchdog();
          break;
        }
      }
    }
    Thread::wait(1); // 1000 Hz
  }
}

void send_msg_thread(void const *args) {
  while (true) {
    bone.serial.printf("0 %d\n", imu->get_angle());
    swervedrive->angle = imu->get_angle();
    bone.serial.printf("1 %d %d %d %d %d %d\n",
      swervedrive->m0_vel_setp, swervedrive->m0_rot_setp,
      swervedrive->m1_vel_setp, swervedrive->m1_rot_setp,
      swervedrive->m2_vel_setp, swervedrive->m2_rot_setp);
    bone.serial.printf("9 %d\n", swervedrive->_enabled);
    Thread::wait(20); // 50 Hz
  }
}

int main() {
  pc.printf("Starting!\r\n");
  // Constructor for classes with threads must be called within main()
  imu = new IMU(PTE0, PTE1, PTE25, PTE24);
  swervedrive = new SwerveDrive(imu,
                                m0_steer, m0_drive,
                                m1_steer, m1_drive,
                                m2_steer, m2_drive);

  // Start msg threads
  Thread getMsgThread(get_msg_thread);
  Thread sendmsgThread(send_msg_thread);

  pc.printf("Startup Complete!\r\n");
  while (true) {
    led = !led;
    Thread::wait(100);
  }
}
