#include "MotorDriver.h"

MotorDriver::MotorDriver(PinName pwm, PinName fwd, PinName rev):
                         _pwm(pwm), _fwd(fwd), _rev(rev) {
  _pwm.period_us(kPwmPeriodUs);
  _pwm.pulsewidth_us(0);
  _fwd = 0;
  _rev = 0;
}

void MotorDriver::setPower(int power) {
	power = constrain(power, -100, 100);
	_pwm.pulsewidth_us(abs(power * kPwmPeriodUs / 100));
	if (power >= 0) {
		_fwd = 1;
		_rev = 0;
	} else {
		_fwd = 0;
		_rev = 1;
	}
}
