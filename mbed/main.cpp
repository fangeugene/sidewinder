#include "defines.h"
#include "mbed.h"
#include "rtos.h"
#include "MODSERIAL.h"
#include "BeagleBone.h"
#include "IMU.h"

MODSERIAL pc(USBTX, USBRX);
BeagleBone bone(PTD3, PTD2);
DigitalOut led(LED_BLUE);

volatile unsigned int i = 0;
void encoder_isr_simulator() {
  led = !led;
  i += 1;
  wait_us(2);
}

void encoder_simulator_printer_thread(void const *args) {
  while (true) {
    pc.printf("Count: %d\r\n", i);
    Thread::wait(1000);
  }
}

void send_data_thread(void const *args) {
  while (true) {
    bone.serial.printf("%d\r\n", imu::get_angle());
    Thread::wait(20);
  }
}

int main() {
    // Temp for testing
    Ticker t;
    t.attach_us(&encoder_isr_simulator, 100);
    Thread encoderSimThread(encoder_simulator_printer_thread);

    // IMU setup
    imu::gyro_calibrate();
    RtosTimer gyro_integration_timer(imu::gyro_integrate);
    gyro_integration_timer.start(imu::kGyroIntegrationMs);

    // Start threads
    Thread sendDataThread(send_data_thread);

    char msg[kMaxMsgSize];
    while (true) {
      Thread::wait(10);
      if (bone.has_msg()) {
        bone.read_msg(msg, kMaxMsgSize);
        bone.serial.printf("ECHO!\r\n");
        bone.serial.printf(msg);
      }
    }
}
