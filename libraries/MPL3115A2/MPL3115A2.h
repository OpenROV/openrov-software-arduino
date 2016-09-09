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

namespace mpl3115a2
{
    // Unshifted 7-bit I2C address for sensor
    static constexpr uint8_t MPL3115A2_ADDRESS = 0x60;

    //TODO
    //We should really have a system wide firmware ret code framework
    enum ERetCode
    {
        SUCCESS,
        FAILED,
        FAILED_ONESHOT,
        FAILED_PRESSURE_READ,
        TIMED_OUT,
        UNKNOWN
    };

    enum EMode
    {
        BAROMETER,
        ALTIMETER,
        STANDBY,
        ACTIVE
    };

    enum EOversampleRatio : uint8_t
    {
        OSR_1   = 0,
        ORS_2   = 1,
        ORS_4   = 2,
        ORS_8   = 3,
        ORS_16  = 4,
        ORS_32  = 5,
        ORS_64  = 6,
        ORS_128 = 7
    };

    class MPL3115A2
    {

        public:

            MPL3115A2( CI2C *i2cInterfaceIn );

            //Public member functions
            ERetCode ReadPressure( float& pressureOut );

            ERetCode Initialize();
            ERetCode SetMode( EMode modeIn );
            ERetCode SetOversampleRatio( EOversampleRatio osrIn );
            ERetCode EnableEventFlags();

            //Public Attributes
            bool IsInitialized() const { return m_isInitialized; };
            
        private:

            enum class MPL3115A2_REGISTER : uint8_t
            {
                STATUS                      = 0x00,
                
                PRESSURE_OUT_MSB            = 0x01,
                PRESSURE_OUT_CSB            = 0x02,
                PRESSURE_OUT_LSB            = 0x03,

                TEMP_OUT_MSB                = 0x04,
                TEMP_OUT_LSB                = 0x05,

                DR_STATUS                   = 0x06,

                PRESSURE_OUT_DELTA_MSB      = 0x07,
                PRESSURE_OUT_DELTA_CSB      = 0x08,
                PRESSURE_OUT_DELTA_LSB      = 0x09,

                TEMP_OUT_DELTA_MSB          = 0x0A,
                TEMP_OUT_DELTA_LSB          = 0x0B,

                WHO_AM_I                    = 0x0C,

                FIFO_STATUS                 = 0x0D,
                FIFO_DATA                   = 0x0E,
                FIFO_SETUP                  = 0x0F,

                TIME_DELAY                  = 0x10,

                SYSMOD                      = 0x11,

                INT_SOURCE                  = 0x12,

                PT_DATA_CFG                 = 0x13,

                BAR_INPUT_MSB               = 0x14,
                BAR_INPUT_LSB               = 0x15,

                PRESSURE_TARGET_MSB         = 0x16,
                PRESSURE_TARGET_LSB         = 0x17,
                
                TEMP_TARGET                 = 0x18,

                PRESSURE_WINDOW_MSB         = 0x19,
                PRESSURE_WINDOW_LSB         = 0x1A,

                TEMP_WINDOW                 = 0x1B,

                MIN_PRESSURE_DATA_OUT_MSB   = 0x1C,
                MIN_PRESSURE_DATA_OUT_CSB   = 0x1D,
                MIN_PRESSURE_DATA_OUT_LSB   = 0x1E,

                MIN_TEMP_DATA_OUT_MSB       = 0x1F,
                MIN_TEMP_DATA_OUT_LSB       = 0x20,

                MAX_PRESSURE_DATA_OUT_MSB   = 0x21,
                MAX_PRESSURE_DATA_OUT_CSB   = 0x22,
                MAX_PRESSURE_DATA_OUT_LSB   = 0x23,

                MAX_TEMP_DATA_OUT_MSB       = 0x24,
                MAX_TEMP_DATA_OUT_LSB       = 0x25,

                CONTROL_REGISTER_1          = 0x26,
                CONTROL_REGISTER_2          = 0x27,
                CONTROL_REGISTER_3          = 0x28,
                CONTROL_REGISTER_4          = 0x29,
                CONTROL_REGISTER_5          = 0x2A,

                PRESSURE_DATA_OFFSET        = 0x2B,
                TEMP_DATA_OFFSET            = 0x2C,
                ALT_DATA_OFFSET             = 0x2D
            };

            //Private member functions
            ERetCode VerifyChipId();
            ERetCode ToggleOneShot();

            ERetCode SetModeBarometer();
            ERetCode SetModeAltimeter();
            ERetCode SetModeStandby();
            ERetCode SetModeActive();

            int32_t ReadByte( MPL3115A2_REGISTER addressIn, uint8_t& dataOut );
            int32_t ReadNBytes( MPL3115A2_REGISTER addressIn, uint8_t* dataOut, uint8_t byteCountIn );
            int32_t WriteByte( MPL3115A2_REGISTER addressIn, uint8_t dataIn );

            //Private member Attributes
            uint8_t m_i2cAddress;
            CI2C *m_pI2C;
            int32_t m_sensorId;

            bool m_isInitialized = false;
    };
}



