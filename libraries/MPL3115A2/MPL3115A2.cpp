/* 
 MPL3115A2 Barometric Pressure Sensor Library
 By: Nathan Seidle
 SparkFun Electronics
 Date: September 24th, 2013
 License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).
 
 Get pressure, altitude and temperature from the MPL3115A2 sensor.
 
 September 6, 2016: Modified for use in OpenROV's Software 

*/

#include <Arduino.h>
#include <CI2C.h>

#include "MPL3115A2.h"

using namespace mpl3115a2;


MPL3115A2::MPL3115A2( CI2C *i2cInterfaceIn, int32_t sensorIdIn = -1, uint8_t addressIn = MPL3115A2_ADDRESS )
    : m_pI2C( i2cInterfaceIn )
    , m_sensorId( sensorIdIn )
    , m_i2cAddress( addressIn )
{

}

int32_t MPL3115A2::Initialize() 
{
    //Verify 


}
