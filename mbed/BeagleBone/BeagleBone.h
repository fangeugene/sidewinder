#ifndef _BEAGLEBONE_H_
#define _BEAGLEBONE_H_
#include "mbed.h"
#include "MODSERIAL.h"

/**
 * Encapsulates a UART connection to the BeagleBone
 *
 * Strings sent to a full buffer will be discarded
 */
class BeagleBone {
 public:
  /**
   * Constructor
   *
   * @param tx The transmit pin name
   * @param rx The receive pin name
   */
  BeagleBone(PinName tx, PinName rx);

  /**
   * Copies a string from the circular buffer into the given buffer
   * @param str The destination for copying
   * @param Maximum number of bytes to copy
   */
  void read_msg(char *str, const int len);
  /**
   * Checks if there are strings available
   * @return Strings are available
   */
  bool has_msg(void);
  /**
   * Writes the string to the bone
   * @param str The string to write
   */
  void write_msg(const char *str);
  MODSERIAL serial_;

 private:
  static const int kBufferLen = 512;
  static const int kBaudRate = 115200;
  static const char kNewlineChar = '\n';

  void rxCallback_(MODSERIAL_IRQ_INFO *q);
  volatile unsigned int pending_msgs_;

};
#endif // _BEAGLEBONE_H_
