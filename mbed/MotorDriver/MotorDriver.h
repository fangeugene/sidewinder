#ifndef _MOTORDRIVER_H_
#define _MOTORDRIVER_H_
#include "defines.h"
#include "mbed.h"

const int kPwmPeriodUs = 100;

class MotorDriver {
  public:
	  MotorDriver(PinName pwm, PinName fwd, PinName rev);
    void setPower(int Power);

  protected:
    PwmOut _pwm;
	  DigitalOut _fwd;
    DigitalOut _rev;
};

#endif // _MOTORDRIVER_H_
