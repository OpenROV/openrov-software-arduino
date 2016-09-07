/* 
 MPL3115A2 Barometric Pressure Sensor Library
 By: Nathan Seidle
 SparkFun Electronics
 Date: September 24th, 2013
 License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).
 
 Get pressure, altitude and temperature from the MPL3115A2 sensor.
 
 September 6, 2016: Modified for use in OpenROV's Software 

*/

#pragma once

//Forward declaration of I2C interface
class CI2C;

class MPL3115A2
{
    public:
        MPL3115A2( CI2C *i2cInterfaceIn, int32_t sensorIdIn = -1, uint8_t addressIn )
    private:
        CI2C *m_pI2C;

}


