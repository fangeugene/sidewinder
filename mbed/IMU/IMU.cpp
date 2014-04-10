#include "IMU.h"

IMU::IMU(PinName gyro_sda, PinName gyro_scl, PinName mag_sda, PinName mag_scl):
         _gyro(gyro_sda, gyro_scl), _mag(mag_sda, mag_scl) {
  // _gyro_calibrate();
}

void IMU::_gyro_calibrate() {
  _gyro_angle = 0;
  _gyro.setLpBandwidth(LPFBW_256HZ);

  // Compute gyro drift
  int sum = 0;
  for (int i=0; i<kGyroCalibCycles; i++) {
      sum += _gyro.getGyroZ();
      wait_ms(5);
  }
  _gyro_offset = sum;
}

void IMU::_gyro_integrate(void const *args) {
  _gyro_angle += (_gyro.getGyroZ() * kGyroCalibCycles - _gyro_offset) / 1444; // TODO: make a constant
}

float IMU::get_angle() {
  return 0;
  int angle = _gyro_angle / 10000; // TODO: make a constant
  if (angle > 180) {
    angle -= 360;
  } else if (angle <= -180) {
    angle += 360;
  }
  return angle;
}
