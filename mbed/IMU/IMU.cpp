#include "IMU.h"

IMU::IMU(PinName gyro_sda, PinName gyro_scl, PinName mag_sda, PinName mag_scl):
         _gyro(gyro_sda, gyro_scl),
         _gyro_integration_timer(IMU::_gyro_integration_static_callback, osTimerPeriodic, this),
         ts(1) {
  _gyro_calibrate();
  _gyro_integration_timer.start(kGyroIntegrationMs);
}

void IMU::reset() {
	_gyro_angle = 0;
}

void IMU::_gyro_integration_static_callback(void const *args) {
  // Static callback -- see https://mbed.org/forum/mbed/topic/4388/ for more info
  IMU *instance = (IMU*)args;
  instance->_gyro_integrate();
}

void IMU::_gyro_calibrate() {
  IMU::reset();
  _gyro.setLpBandwidth(LPFBW_256HZ);

  // Compute gyro drift
  int sum = 0;
  for (int i=0; i<kGyroCalibCycles; i++) {
      sum += _gyro.getGyroZ();
      wait_ms(5);
  }
  _gyro_offset = sum;
}

void IMU::_gyro_integrate() {
  _gyro_angle += (_gyro.getGyroZ() * kGyroCalibCycles - _gyro_offset) / 1444; // TODO: make a constant
}

int IMU::get_angle() {
  // ts.wait();
  int angle = _gyro_angle / 10000; // TODO: make a constant
  // if (angle > 180) {
  //   angle -= 360;
  // } else if (angle <= -180) {
  //   angle += 360;
  // }
  // ts.release();
  return angle + 180;
}
