#include "defines.h"
#include "mbed.h"
#include "rtos.h"
#include "MODSERIAL.h"
#include "BeagleBone.h"
#include "IMU.h"
#include "SwerveDrive.h"

MODSERIAL pc(USBTX, USBRX);
BeagleBone bone(PTD3, PTD2);
DigitalOut led(LED_BLUE);

// Temp for testing
volatile unsigned int i = 0;
unsigned int n = 0;
void encoder_isr_simulator() {
  i += 1;
  wait_us(2);
}

void encoder_simulator_printer_thread(void const *args) {
  while (true) {
    pc.printf("Count: %d, %d\r\n", i, n);
    Thread::wait(10000);
  }
}
// End temp for testing

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
        }
        case '1': {
          int t_mag_setp = (msg[2] - '0') * 100 + (msg[3] - '0') * 10 + (msg[4] - '0');
          int t_head_setp = (msg[6] - '0') * 100 + (msg[7] - '0') * 10 + (msg[8] - '0');
          t_head_setp -= 500;
          int rot_setp = 0;
          swervedrive::set_setpoints(t_mag_setp, t_head_setp, rot_setp);
        }
      }
    }
    Thread::wait(1); // 1000 Hz
  }
}

void send_msg_thread(void const *args) {
  while (true) {
    bone.serial.printf("0 %d\n", imu::get_angle());
    bone.serial.printf("1 %d %d %d %d %d %d\n",
      swervedrive::read_r(0), swervedrive::read_theta(0),
      swervedrive::read_r(1), swervedrive::read_theta(1),
      swervedrive::read_r(2), swervedrive::read_theta(2));
    //pc.printf("Module 0: %d, %d\r\n", swervedrive::read_r(0), swervedrive::read_theta(0));
    //pc.printf("Module 1: %d, %d\r\n", swervedrive::read_r(1), swervedrive::read_theta(1));
    //pc.printf("Module 2: %d, %d\r\n", swervedrive::read_r(2), swervedrive::read_theta(2));
    Thread::wait(20); // 50 Hz
  }
}

int main() {
  // Temp for testing
  Ticker t;
  t.attach_us(&encoder_isr_simulator, 30);
  Thread encoderSimThread(encoder_simulator_printer_thread);
  // End temp for testing

  // IMU setup
  imu::gyro_calibrate();
  RtosTimer gyro_integration_timer(imu::gyro_integrate);
  gyro_integration_timer.start(imu::kGyroIntegrationMs);
  
  // SwerveDrive setup
  swervedrive::init();
  RtosTimer module_control_timer(swervedrive::module_control);
  module_control_timer.start(swervedrive::kModuleControlPeriodMs);

  // Start threads
  Thread getMsgThread(get_msg_thread);
  Thread sendmsgThread(send_msg_thread);
  
  Timer m;
  m.start();
  swervedrive::calculate_module_setpoints(0, 1.2, 2.3, 3.4, 4.5, 5.6);
  m.stop();
  pc.printf("Time: %d\r\n", m.read_us());

  while (true) {
    n += 1;
  }
}
