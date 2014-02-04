#include "IMU.h"

namespace imu {
  ITG3200 g_gyro(PTC9, PTC8);
  int g_gyro_offset;
  int g_gyro_angle;

  void gyro_calibrate() {
    g_gyro_angle = 0;
    g_gyro.setLpBandwidth(LPFBW_256HZ);

    // Compute gyro drift
    int sum = 0;
    for (int i=0; i<kGyroCalibCycles; i++) {
        sum += g_gyro.getGyroZ();
        wait_ms(5);
    }
    g_gyro_offset = sum;
  }

  void gyro_integrate(void const *args) {
    g_gyro_angle += (g_gyro.getGyroZ() * kGyroCalibCycles - g_gyro_offset) / 1444; // TODO: make a constant
  }

  int get_angle() {
    int angle = g_gyro_angle / 10000; // TODO: make a constant
    if (angle > 180) {
      angle -= 360;
    } else if (angle <= -180) {
      angle += 360;
    }
    return angle;
  }
}
