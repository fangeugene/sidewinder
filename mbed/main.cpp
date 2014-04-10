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
DigitalOut led(LED2);

MotorDriver m0_steer(PTB1, PTA6, PTD6); // 2
MotorDriver m0_drive(PTB2, PTA7, PTD4); // 3
MotorDriver m1_steer(PTB3, PTC16, PTD2); // 4
MotorDriver m1_drive(PTC2, PTC13, PTA13);   // 5
MotorDriver m2_steer(PTE31, PTA15, PTD5);  // 0
MotorDriver m2_drive(PTB0, PTA14, PTD7);  // 1

SwerveDrive* swervedrive;
IMU* imu;

// // Temp for testing
// volatile unsigned int i = 0;
// unsigned int n = 0;
// void encoder_isr_simulator() {
//   i += 1;
//   wait_us(2);
// }

// void encoder_simulator_printer_thread(void const *args) {
//   while (true) {
//     pc.printf("Count: %d, %d\r\n", i, n);
//     Thread::wait(10000);
//   }
// }
// // End temp for testing

 void get_msg_thread(void const *args) {
   char msg[kMaxMsgSize];
   while (true) {
     if (bone.has_msg()) {
       bone.read_msg(msg, kMaxMsgSize);
       switch (msg[0]) {
         case '0': {
           if (msg[2] == '0') {
             led = 0;
           } else if (msg[2] == '1') {
             led = 1;
           }
           break;
         }
         case '1': {
           int t_mag_setp = (msg[2] - '0') * 100 + (msg[3] - '0') * 10 + (msg[4] - '0');
           int t_head_setp = (msg[6] - '0') * 100 + (msg[7] - '0') * 10 + (msg[8] - '0');
           t_head_setp -= 500;
           int rot_setp = 0;
           swervedrive->set_setpoints(t_mag_setp, t_head_setp, rot_setp);
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
     bone.serial.printf("1 %d %d %d %d %d %d\n",
       swervedrive->m0_vel_setp, swervedrive->m0_rot_setp,
       swervedrive->m1_vel_setp, swervedrive->m1_rot_setp,
       swervedrive->m2_vel_setp, swervedrive->m2_rot_setp);
     // pc.printf("Module 0: %d, %d\r\n", swervedrive->m0_vel_setp, swervedrive->m0_rot_setp);
     // pc.printf("Module 1: %d, %d\r\n", swervedrive->m1_vel_setp, swervedrive->m1_rot_setp);
     // pc.printf("Module 2: %d, %d\r\n", swervedrive->m2_vel_setp, swervedrive->m2_rot_setp);
     Thread::wait(20); // 50 Hz
   }
 }

int main() {
  // Constructor for classes with threads must be called within main()
  imu = new IMU(PTC9, PTC8, PTE25, PTE24);
  swervedrive = new SwerveDrive(imu,
                                m0_steer, m0_drive,
                                m1_steer, m1_drive,
                                m2_steer, m2_drive);

  // Temp for testing
  //Ticker t;
  //t.attach_us(&encoder_isr_simulator, 30);
  //Thread encoderSimThread(encoder_simulator_printer_thread);
  // End temp for testing

  // IMU setup
  // imu::gyro_calibrate();
  // RtosTimer gyro_integration_timer(imu::gyro_integrate);
  // gyro_integration_timer.start(imu::kGyroIntegrationMs);

  // Start threads
  Thread getMsgThread(get_msg_thread);
  Thread sendmsgThread(send_msg_thread);

  pc.printf("Starting!\r\n");
  while (true) {
    led = !led;
    Thread::wait(1000);
  }
}
