/***************************************************************************
  This is a library for the BMP280 pressure sensor

  Designed specifically to work with the Adafruit BMP280 Breakout
  ----> http://www.adafruit.com/products/2651

  These sensors use I2C to communicate, 2 pins are required to interface.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!

  Written by Kevin Townsend for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ***************************************************************************/

#include "BMP280.h"
#include <CI2C.h>

BMP280::BMP280( CI2C *i2cInterfaceIn )
  : m_pI2C( i2cInterfaceIn )
  , m_i2cAddress( BMP280_ADDRESS )
{
}

int32_t BMP280::Initialize()
{

  int32_t ret = 0;
  uint8_t chipId = 0x00;

  ret = ReadByte( BMP280_REGISTER_CHIPID, chipId );

  if( ret || chipId != 0x58 )
  {
    return -1;
  }
  
  if( ReadCoefficients() )
  {
    return -1;
  }

  // Set temp oversampling to 2x, pres oversampling to 16x, and mode to normal. Good resolution at 10hz
  ret = WriteByte( BMP280_REGISTER_CONTROL, 0x57 );

  return ret;
}


int32_t BMP280::WriteByte( uint8_t registerIn, uint8_t dataIn )
{
	return (int32_t)m_pI2C->WriteByte( m_i2cAddress, (uint8_t)registerIn, dataIn );
}

int32_t BMP280::ReadByte( uint8_t addressIn, uint8_t &dataOut )
{
	return (int32_t)m_pI2C->ReadByte( m_i2cAddress, (uint8_t)addressIn, dataOut );
}

int32_t BMP280::ReadNBytes( uint8_t addressIn, uint8_t *dataOut, uint8_t byteCountIn )
{
	return (int32_t)m_pI2C->ReadBytes( m_i2cAddress, (uint8_t)addressIn, dataOut, byteCountIn );
}

int32_t BMP280::ReadWord( uint8_t addressIn, uint16_t &dataOut )
{
	return (int32_t)m_pI2C->ReadWord( m_i2cAddress, (uint8_t)addressIn, dataOut );
}

int32_t BMP280::ReadWord_Signed( uint8_t addressIn, int16_t &dataOut )
{
	return (int32_t)m_pI2C->ReadWord( m_i2cAddress, (uint8_t)addressIn, (uint16_t&)dataOut );
}

int32_t BMP280::ReadWord_LE( uint8_t addressIn, uint16_t &dataOut )
{
  uint16_t temp = 0;

  // The sample trimmer registers are ordered LSB/MSB, so this allows you to read them as MSB/LSB
  int32_t ret = (int32_t)m_pI2C->ReadWord( m_i2cAddress, (uint8_t)addressIn, temp );

  // Flip the bytes
  dataOut = ( temp >> 8 ) | ( temp << 8 );

	return ret;
}

int32_t BMP280::ReadWord_Signed_LE( uint8_t addressIn, int16_t &dataOut )
{
  uint16_t temp = 0;

  // The sample trimmer registers are ordered LSB/MSB, so this allows you to read them as MSB/LSB
  int32_t ret = (int32_t)m_pI2C->ReadWord( m_i2cAddress, (uint8_t)addressIn, temp );

  // Flip the bytes
  dataOut = (int16_t)( (temp >> 8) | (temp << 8) );

	return ret;
}

int32_t BMP280::ReadCoefficients()
{
    int32_t ret = 0;
    ret |= ReadWord_LE( BMP280_REGISTER_DIG_T1, m_calibData.dig_T1 );
    ret |= ReadWord_Signed_LE( BMP280_REGISTER_DIG_T2, m_calibData.dig_T2 );
    ret |= ReadWord_Signed_LE( BMP280_REGISTER_DIG_T3, m_calibData.dig_T3 );

    ret |= ReadWord_LE( BMP280_REGISTER_DIG_P1, m_calibData.dig_P1 );
    ret |= ReadWord_Signed_LE( BMP280_REGISTER_DIG_P2, m_calibData.dig_P2 );
    ret |= ReadWord_Signed_LE( BMP280_REGISTER_DIG_P3, m_calibData.dig_P3 );
    ret |= ReadWord_Signed_LE( BMP280_REGISTER_DIG_P4, m_calibData.dig_P4 );
    ret |= ReadWord_Signed_LE( BMP280_REGISTER_DIG_P5, m_calibData.dig_P5 );
    ret |= ReadWord_Signed_LE( BMP280_REGISTER_DIG_P6, m_calibData.dig_P6 );
    ret |= ReadWord_Signed_LE( BMP280_REGISTER_DIG_P7, m_calibData.dig_P7 );
    ret |= ReadWord_Signed_LE( BMP280_REGISTER_DIG_P8, m_calibData.dig_P8 );
    ret |= ReadWord_Signed_LE( BMP280_REGISTER_DIG_P9, m_calibData.dig_P9 );

    return ret;
}

int32_t BMP280::ReadRawPressureAndTemperature( int32_t &rawPresOut, int32_t &rawTempOut )
{
  // Read raw pressure and temperature
  uint8_t buf[ 6 ] = { 0 };

  int32_t ret = (int32_t)m_pI2C->ReadBytes( m_i2cAddress, BMP280_REGISTER_PRESSUREDATA, buf, 6 );

  rawPresOut = (int32_t)( ( ((uint32_t)(buf[0])) << 12 ) |
                          ( ((uint32_t)(buf[1])) << 4 ) |
                          ( ((uint32_t)(buf[2])) >> 4 ) );

  rawTempOut = (int32_t)( ( ((uint32_t)(buf[3])) << 12 ) |
                          ( ((uint32_t)(buf[4])) << 4 ) |
                          ( ((uint32_t)(buf[5])) >> 4 ) );

  return ret;
}

int32_t BMP280::ReadCompensatedPressureAndTemperature( float &presOut, float &tempOut )
{
  int32_t adc_P, adc_T;

  int32_t ret = ReadRawPressureAndTemperature( adc_P, adc_T );

  if( !ret )
  {
    // ----------------------
    // Calculate Compensated Temperature

    int32_t var1 = ( ( ( ( adc_T >> 3 ) - ( (int32_t)m_calibData.dig_T1 << 1 ) ) ) * ((int32_t)m_calibData.dig_T2) ) >> 11;
    int32_t var2 = ( ( ( ( ( adc_T >> 4 ) - ( (int32_t)m_calibData.dig_T1 ) ) * ( ( adc_T >> 4 ) - ( (int32_t)m_calibData.dig_T1 ) ) ) >> 12 ) * ( (int32_t)m_calibData.dig_T3 ) ) >> 14;

    m_tFine = var1 + var2;

    tempOut = (float)( ( m_tFine * 5 + 128 ) >> 8 ) / 100.0f;

    // ----------------------
    // Calculate Compensated Pressure

    int64_t var3 = ( (int64_t)m_tFine ) - 128000;
    int64_t var4 = var3 * var3 * (int64_t)m_calibData.dig_P6;

    var4 = var4 + ((var3*(int64_t)m_calibData.dig_P5)<<17);
    var4 = var4 + (((int64_t)m_calibData.dig_P4)<<35);
    var3 = ((var3 * var3 * (int64_t)m_calibData.dig_P3)>>8) + ((var3 * (int64_t)m_calibData.dig_P2)<<12);
    var3 = (((((int64_t)1)<<47)+var3))*((int64_t)m_calibData.dig_P1)>>33;

    // Avoid exception caused by division by zero
    if( var3 == 0 ) 
    {
      return -1;  
    }

    int64_t p = 1048576 - adc_P;

    p = (((p<<31) - var4)*3125) / var3;

    var3 = (((int64_t)m_calibData.dig_P9) * (p>>13) * (p>>13)) >> 25;
    var4 = (((int64_t)m_calibData.dig_P8) * p) >> 19;

    p = ((p + var3 + var4) >> 8) + (((int64_t)m_calibData.dig_P7)<<4);

    presOut = (float)p / 256.0f;
  }

  return ret;
}