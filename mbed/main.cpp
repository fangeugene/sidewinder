#include "mbed.h"
#include "rtos.h"
#include "ITG3200.h"
#include "MODSERIAL.h"
#include "BeagleBone.h"

ITG3200 gyro(PTC9, PTC8);
MODSERIAL pc(USBTX, USBRX);
BeagleBone bone(PTD3, PTD2);
DigitalOut led(LED_BLUE);

int g_gyro_total;
int g_gyro_offset;
int GYRO_CALIB_CYCLES = 1000;

void gyro_setup() {
    pc.printf("Setting up gyro...\r\n");
    gyro.setLpBandwidth(LPFBW_256HZ);

    pc.printf("Beginning gyro calibration...\r\n");
    int sum = 0;
    for (int i=0; i<GYRO_CALIB_CYCLES; i++) {
        sum += gyro.getGyroZ();
        if (i % (GYRO_CALIB_CYCLES / 10) == 0) {
          pc.printf("%d / 10\r\n", i * 10 / GYRO_CALIB_CYCLES);
        }
        wait_ms(5);
    }
    g_gyro_offset = sum;
    pc.printf("Gyro offset: %d\r\n", g_gyro_offset);
}

void gyro_integration(void const *args) {
    g_gyro_total += gyro.getGyroZ()*GYRO_CALIB_CYCLES - g_gyro_offset;
}

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
    Thread thread(encoder_simulator_printer_thread);
    
    Ticker t;
    t.attach_us(&encoder_isr_simulator, 100);

    g_gyro_total = 0;
    //gyro_setup();

    //RtosTimer gyro_integration_timer(gyro_integration, osTimerPeriodic, (void *)0);
    //gyro_integration_timer.start(1);

    char msg[100];
    while(1) {
      Thread::wait(10);
      if (bone.has_msg()) {
        bone.read_msg(msg, 100);
        bone.write_msg("ECHO!\r\n");
        bone.write_msg(msg);
      }
    }
}
