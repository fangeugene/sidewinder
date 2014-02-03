#include "defines.h"
#include "mbed.h"
#include "rtos.h"
#include "MODSERIAL.h"
#include "BeagleBone.h"
#include "IMU.h"

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
    Thread::wait(1000);
  }
}
// End temp for testing

void get_msg_thread(void const *args) {
  char msg[kMaxMsgSize];
  while (true) {
    if (bone.has_msg()) {
      bone.read_msg(msg, kMaxMsgSize);
      pc.printf("Message received: %s\r\n", msg);
      switch (msg[0]) {
        case '0': {
          if (msg[2] == '0') {
            led = 0;
          } else if (msg[2] == '1') {
            led = 1;
          }
        }
      }
    }
    Thread::wait(1); // 1000 Hz
  }
}

void send_msg_thread(void const *args) {
  while (true) {
    bone.serial.printf("%d\n", imu::get_angle());
    Thread::wait(20); // 50 Hz
  }
}

int main() {
  // Temp for testing
  Ticker t;
  t.attach_us(&encoder_isr_simulator, 100);
  Thread encoderSimThread(encoder_simulator_printer_thread);
  // End temp for testing

  // IMU setup
  imu::gyro_calibrate();
  RtosTimer gyro_integration_timer(imu::gyro_integrate);
  gyro_integration_timer.start(imu::kGyroIntegrationMs);

  // Start threads
  Thread getMsgThread(get_msg_thread);
  Thread sendmsgThread(send_msg_thread);

  while (true) {
    n += 1;
  }
}
