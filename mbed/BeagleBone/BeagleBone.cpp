#include "BeagleBone.h"

BeagleBone::BeagleBone(PinName tx, PinName rx) : serial(tx, rx, kBufferLen) {
  pending_msgs_ = 0;
  serial.baud(kBaudRate);
  serial.attach(this, &BeagleBone::rxCallback_, MODSERIAL::RxIrq);
}

bool BeagleBone::has_msg(void) {
  return pending_msgs_ > 0;
}

void BeagleBone::read_msg(char *str, const int len) {
  char c = 0;
  int i;
  for (i = 0; c != kNewlineChar && i < len; i++) {
    c = serial.getc();
    str[i] = c;
  }
  str[i] = 0; // end of string
  pending_msgs_ -= 1;
}

void BeagleBone::rxCallback_(MODSERIAL_IRQ_INFO *q) {
  MODSERIAL *serial = q->serial;
  if (serial->rxGetLastChar() == kNewlineChar) {
    pending_msgs_ += 1;
  }
}

