/*
 * MAG3110 Sensor Library for mbed
 * TODO: Add proper header
 */

#ifndef MAG3110_H
#define MAG3110_H

#include "mbed.h"

#define PI 3.14159265359

#define MAG_ADDR 0x1D

// define registers
#define MAG_DR_STATUS 0x00
#define MAG_OUT_X_MSB 0x01
#define MAG_OUT_X_LSB 0x02
#define MAG_OUT_Y_MSB 0x03
#define MAG_OUT_Y_LSB 0x04
#define MAG_OUT_Z_MSB 0x05
#define MAG_OUT_Z_LSB 0x06
#define MAG_WHO_AM_I  0x07
#define MAG_SYSMOD    0x08
#define MAG_OFF_X_MSB 0x09
#define MAG_OFF_X_LSB 0x0A
#define MAG_OFF_Y_MSB 0x0B
#define MAG_OFF_Y_LSB 0x0C
#define MAG_OFF_Z_MSB 0x0D
#define MAG_OFF_Z_LSB 0x0E
#define MAG_DIE_TEMP  0x0F
#define MAG_CTRL_REG1 0x10
#define MAG_CTRL_REG2 0x11

// what should WHO_AM_I return?
#define MAG_3110_WHO_AM_I_VALUE 0xC4


// Fields in registers
// CTRL_REG1: dr2,dr1,dr0  os1,os0  fr tm ac

// Sampling rate from 80Hz down to 0.625Hz
#define MAG_3110_SAMPLE80 0
#define MAG_3110_SAMPLE40 0x20
#define MAG_3110_SAMPLE20 0x40
#define MAG_3110_SAMPLE10 0x60
#define MAG_3110_SAMPLE5 0x80
#define MAG_3110_SAMPLE2_5 0xA0
#define MAG_3110_SAMPLE1_25 0xC0
#define MAG_3110_SAMPLE0_625 0xE0

// How many samples to average (lowers data rate)
#define MAG_3110_OVERSAMPLE1 0
#define MAG_3110_OVERSAMPLE2 0x08
#define MAG_3110_OVERSAMPLE3 0x10
#define MAG_3110_OVERSAMPLE4 0x18

// read only 1 byte per axis
#define MAG_3110_FASTREAD 0x04
// do one measurement (even if in standby mode)
#define MAG_3110_TRIGGER 0x02
// put in active mode
#define MAG_3110_ACTIVE 0x01

// CTRL_REG2: AUTO_MRST_EN  _ RAW MAG_RST _ _ _ _ _
// reset sensor after each reading
#define MAG_3110_AUTO_MRST_EN 0x80
// don't subtract user offsets
#define MAG_3110_RAW 0x20
// reset magnetic sensor after too-large field
#define MAG_3110_MAG_RST 0x10

// DR_STATUS Register ZYXOW ZOW YOW XOW ZYXDR ZDR YDR XDR
#define MAG_3110_ZYXDR  0x08

/**
 * MAG3110 Class to read X/Y/Z data from the magentometer
 *
 */
class MAG3110
{
public:
    /**
     * Main constructor
     * @param sda SDA pin
     * @param sdl SCL pin
     * @param addr addr of the I2C peripheral
     */
    MAG3110(PinName sda, PinName scl);
    /**
     * Debug version of constructor
     * @param sda SDA pin
     * @param sdl SCL pin
     * @param addr Address of the I2C peripheral
     * @param pc Serial object to output debug messages
     */
    MAG3110(PinName sda, PinName scl, Serial *pc); //pass serial for debug
    /**
     * Setup the Magnetometer
     *
     */
    void begin();
    /**
     * Read a register, return its value as int
     * @param regAddr The address to read
     * @return value in register
     */
    int readReg(char regAddr);
    /**
     * Read a value from a pair of registers, return as int
     * @param regAddr The address to read
     * @return Value from 2 consecutive registers
     */
    int readVal(char regAddr);
    /**
     * Calculate the heading
     * @return heading in degrees
     */
    float getHeading();
    /**
     * Perform a read on the X, Y and Z values.
     * @param xVal Pointer to X value
     * @param yVal Pointer to Y value
     * @param zVal Pointer to Z value
     */
    void getValues(int *xVal, int *yVal, int *zVal);
    /**
     * Set the calibration parameters if required.
     * @param minX Minimum value for X range
     * @param maxX Maximum value for X range
     * @param minY Minimum value for Y range
     * @param maxY maximum value for Y range
     */
    void setCalibration(int minX, int maxX, int minY, int maxY);

private:
    I2C _i2c;
    int _i2c_address;
    Serial *_pc;
    bool _debug;
    int _avgX, _avgY;

};
#endif
