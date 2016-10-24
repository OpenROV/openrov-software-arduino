#pragma once

// Driver definitions and helpers
#include "BNO055_Def.h"

namespace bno055
{
    class BNO055
    {
    public:
        // Attributes
        TData           m_data;
        TCalibration    m_calibration;

        // Methods
        BNO055( I2C *i2cInterfaceIn, EAddress addressIn );

        // State machine functions
        void Tick();
        void HardReset();
        void FullReset();
        void Disable();

        bool IsEnabled();

        // Configuration
        EResult SetMode( EMode modeIn );
        EMode   GetMode();

        uint32_t GetUpdatePeriod();

        // Statistics
        uint32_t GetResultCount( EResult resultTypeIn );
        void ClearResultCount( EResult resultTypeIn );

    private:
        // I2C info
        I2C                     *m_pI2C;
        uint8_t                 m_address;

        // Configuration
        EMode                   m_mode          = EMode::GYRO;

        // Status
        bool                    m_enabled       = true;

        EOpMode                 m_opMode;
        uint8_t                 m_chipID;
        uint8_t                 m_postResults;
        ESystemStatus           m_sysStatus;
        ESystemError            m_sysError;
        TRevisionInfo           m_revInfo;

        // State management
        EState                  m_state         = EState::POWER_ON_RESET;
        TTransitionDelay        m_delay;
        
        orutil::CTimer          m_calibTimer;
        orutil::CTimer          m_dataTimer;

        // Result statistics
        orutil::TResultCount<EResult::_RESULT_COUNT, EResult::RESULT_SUCCESS> m_results;

        // Methods
        void Transition( EState nextState );
        void DelayedTransition( EState nextState, uint32_t millisIn );

        EResult Cmd_Reset();

        EResult Cmd_VerifyChipID();
        EResult Cmd_VerifyPostResults();

        EResult Cmd_ReadSystemStatus(); //
        EResult Cmd_ReadSystemError();  //

        EResult Cmd_ReadRevisionInfo();
        EResult Cmd_ReadOperatingMode();
        EResult Cmd_ReadCalibrationState();

        EResult Cmd_ReadEulerData();

        EResult Cmd_ConfigureSensor();
        EResult Cmd_SetOperatingMode( EOpMode opModeIn );

        // I2C Methods
        i2c::EI2CResult WriteRegisterByte( uint8_t registerIn, uint8_t dataIn );
        i2c::EI2CResult ReadRegisterByte( uint8_t registerIn, uint8_t *dataOut );
        i2c::EI2CResult ReadRegisterBytes( uint8_t registerIn, uint8_t *dataOut, uint8_t numBytesIn );
    };
}