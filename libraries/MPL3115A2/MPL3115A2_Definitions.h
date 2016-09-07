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

namespace mpl
{
    enum ERegisterAddress : uint8_t
    {
        // Unshifted 7-bit I2C address for sensor
        MPL3115A2_ADDRESS = 0x60,

        

    }
}