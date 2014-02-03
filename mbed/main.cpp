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

int main() {
    Ticker t;
    t.attach_us(&encoder_isr_simulator, 100);
    Thread thread(encoder_simulator_printer_thread);

    // IMU setup
    imu::gyro_calibrate();
    RtosTimer gyro_integration_timer(imu::gyro_integrate);
    gyro_integration_timer.start(imu::kGyroIntegrationMs);

    char msg[kMaxMsgSize];
    while(1) {
      Thread::wait(10);
      if (bone.has_msg()) {
        bone.read_msg(msg, kMaxMsgSize);
        bone.write_msg("ECHO!\r\n");
        bone.write_msg(msg);
      }
      pc.printf("%d\r\n", imu::get_angle());
    }
}
