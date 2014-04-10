
#include "MAG3110.h"
#include "mbed.h"

/******************************************************************************
 * Constructors
 ******************************************************************************/
MAG3110::MAG3110(PinName sda, PinName scl): _i2c(sda, scl), 
    _i2c_address(0x0E<<1), _pc(NULL), _debug(false)
{
    begin();
}

MAG3110::MAG3110(PinName sda, PinName scl, Serial *pc): _i2c(sda, scl), 
   _i2c_address(0x0E<<1), _pc(pc), _debug(true)
{
    begin();
}

void MAG3110::begin()
{
    char cmd[2];

    cmd[0] = MAG_CTRL_REG2;
    cmd[1] = 0x80;
    _i2c.write(_i2c_address, cmd, 2);

    cmd[0] = MAG_CTRL_REG1;
    cmd[1] = MAG_3110_SAMPLE80+MAG_3110_OVERSAMPLE2+MAG_3110_ACTIVE;
    _i2c.write(_i2c_address, cmd, 2);
    
    // No adjustment initially
    _avgX = 0;
    _avgY = 0;
}

// Read a single byte form 8 bit register, return as int
int MAG3110::readReg(char regAddr)
{
    char cmd[1];

    cmd[0] = regAddr;
    _i2c.write(_i2c_address, cmd, 1);

    cmd[0] = 0x00;
    _i2c.read(_i2c_address, cmd, 1);
    return (int)( cmd[0]);
}


// read a register per, pass first reg value, reading 2 bytes increments register
// Reads MSB first then LSB
int MAG3110::readVal(char regAddr)
{
    char cmd[2];

    cmd[0] = regAddr;
    _i2c.write(_i2c_address, cmd, 1);

    cmd[0] = 0x00;
    cmd[1] = 0x00;
    _i2c.read(_i2c_address, cmd, 2);
    return (int)( (cmd[1]|(cmd[0] << 8))); //concatenate the MSB and LSB
}


float MAG3110::getHeading()
{
    int xVal = readVal(MAG_OUT_X_MSB);
    int yVal = readVal(MAG_OUT_Y_MSB);
    return (atan2((double)(yVal - _avgY),(double)(xVal - _avgX)))*180/PI;
}

void MAG3110::getValues(int *xVal, int *yVal, int *zVal)
{
    *xVal = readVal(MAG_OUT_X_MSB);
    *yVal = readVal(MAG_OUT_Y_MSB);
    *zVal = readVal(MAG_OUT_Z_MSB);
}


void MAG3110::setCalibration(int minX, int maxX, int minY, int maxY )
{
    _avgX=(maxX+minX)/2;
    _avgY=(maxY+minY)/2;
}





