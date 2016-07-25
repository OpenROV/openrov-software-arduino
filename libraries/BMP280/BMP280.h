#pragma once

#include <Arduino.h>
#include <CI2C.h>

#define BMP280_ADDRESS (0x77)

namespace bmp280
{
  enum ERegisterAddress : uint8_t
  {
    BMP280_REGISTER_DIG_T1              = 0x88,
    BMP280_REGISTER_DIG_T2              = 0x8A,
    BMP280_REGISTER_DIG_T3              = 0x8C,

    BMP280_REGISTER_DIG_P1              = 0x8E,
    BMP280_REGISTER_DIG_P2              = 0x90,
    BMP280_REGISTER_DIG_P3              = 0x92,
    BMP280_REGISTER_DIG_P4              = 0x94,
    BMP280_REGISTER_DIG_P5              = 0x96,
    BMP280_REGISTER_DIG_P6              = 0x98,
    BMP280_REGISTER_DIG_P7              = 0x9A,
    BMP280_REGISTER_DIG_P8              = 0x9C,
    BMP280_REGISTER_DIG_P9              = 0x9E,

    BMP280_REGISTER_CHIPID             = 0xD0,
    BMP280_REGISTER_VERSION            = 0xD1,
    BMP280_REGISTER_SOFTRESET          = 0xE0,

    BMP280_REGISTER_CAL26              = 0xE1,  // R calibration stored in 0xE1-0xF0

    BMP280_REGISTER_CONTROL            = 0xF4,
    BMP280_REGISTER_CONFIG             = 0xF5,
    BMP280_REGISTER_PRESSUREDATA       = 0xF7,
    BMP280_REGISTER_TEMPDATA           = 0xFA
  };

  typedef struct
  {
    uint16_t dig_T1;
    int16_t  dig_T2;
    int16_t  dig_T3;

    uint16_t dig_P1;
    int16_t  dig_P2;
    int16_t  dig_P3;
    int16_t  dig_P4;
    int16_t  dig_P5;
    int16_t  dig_P6;
    int16_t  dig_P7;
    int16_t  dig_P8;
    int16_t  dig_P9;

    uint8_t  dig_H1;
    int16_t  dig_H2;
    uint8_t  dig_H3;
    int16_t  dig_H4;
    int16_t  dig_H5;
    int8_t   dig_H6;
  } TCalibrationData;
};

class CI2C;

class BMP280
{
  public:
    BMP280( CI2C *i2cInterfaceIn );

    int32_t Initialize();

    float ReadTemperature();
    float ReadPressure();
    float ReadAltitude( float seaLevelhPa = 1013.25f );

  private:

    CI2C *m_pI2C;

    int32_t ReadCoefficients();

    void      WriteByte();
    int32_t   Read8U(byte reg);
    int32_t   Read16U(byte reg);
    int32_t   Read24U(byte reg);

    int32_t   read16S(byte reg);
    int32_t   read16_LE(byte reg); // little endian
    int32_t   read16S_LE(byte reg); // little endian

    uint8_t   m_i2cAddress;
    int32_t   t_fine;

    bmp280::TCalibrationData m_calibData;

};

#endif
