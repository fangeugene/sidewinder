#include "IMU.h"

void test(void const *args) {
  int a = 0;
  a += 1;
  a = a * 2;
}

IMU::IMU(PinName sda, PinName scl) : gyro_(sda, scl), gyro_integration_timer_(test) {
  gyro_angle_ = 0;
  gyro_.setLpBandwidth(kGyroLpBandwidth);

  // Compute gyro drift
  int sum = 0;
  for (int i=0; i<kGyroCalibCycles; i++) {
      sum += gyro_.getGyroZ();
      wait_ms(5);
  }
  gyro_offset_ = sum;

  // Start gyro integration ticker
  //gyro_integration_timer_.start(1);
  //gyro_integration_ticker_.attach_us(this, &IMU::integrate_gyro, kGyroIntTimeUs);
}

void IMU::gyro_integration_timer_starter(void const *p) {
  int a = 0;
  a += 1;
  a = a * 2;
  IMU *instance = (IMU*)p;
  instance->integrate_gyro(NULL);
}

void IMU::integrate_gyro(void const *args) {
  gyro_angle_ += (gyro_.getGyroZ() * kGyroCalibCycles - gyro_offset_) / 1444; // TODO: make a constant
}

int IMU::get_angle() {
  return gyro_angle_ / 10000; // TODO: make a constant
}
