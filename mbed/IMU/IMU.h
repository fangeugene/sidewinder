#ifndef _IMU_H_
#define _IMU_H_
#include "defines.h"
#include "mbed.h"
#include "rtos.h"
#include "ITG3200.h"

/**
 * Encapsulates an interface to the gyro
 */
class IMU {
 public:
  /**
   * Constructor
   *
   * @param sda The gyro I2C serial data line pin
   * @param scl The gyro I2C serial clock pin
   */
  IMU(PinName sda, PinName scl);

  /**
   * Integrates gyro rate to get position
   */
  void integrate_gyro(void const *n);
  /**
   * @return int angle
   */
  int get_angle(void);

 private:
  static const int kGyroCalibCycles = 1000;
  static const char kGyroLpBandwidth = LPFBW_256HZ;
  static const int kGyroIntTimeUs = 1000;

  ITG3200 gyro_;
  //Ticker gyro_integration_ticker_;
  RtosTimer gyro_integration_timer_;
  static void gyro_integration_timer_starter(void const *p);
  volatile int gyro_angle_;
  volatile int gyro_offset_;

};
#endif // _BEAGLEBONE_H_
