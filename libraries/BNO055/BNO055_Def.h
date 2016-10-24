#pragma once

#include <Arduino.h>
#include <I2C.h>
#include <orutil.h>

namespace bno055
{
	// I2C Slave Addresses
    constexpr uint8_t I2C_ADDRESS_A         = 0x28;
    constexpr uint8_t I2C_ADDRESS_B         = 0x29;

	constexpr uint8_t BNO_ID				= 0xA0;

	constexpr uint32_t kRetryDelay_ms       	= 1000;
	constexpr uint32_t kPowerOnResetDelay_ms    = 1000;     // Data sheet specifies "at least 650ms"
	constexpr uint32_t kModeSwitch_ms           = 30;       // Data sheet specifies 19ms for N->Config and 7ms for Config->N. Allowing some leeway for both.
	constexpr uint32_t kCalUpdateRate_ms        = 1000;

	enum class EAddress
    {
        ADDRESS_A,
        ADDRESS_B
    };

	enum ERegister : uint8_t
	{
		/* Page id register definition */
		REG_PAGE_ID_ADDR                                     = 0X07,

		/* PAGE0 REGISTER DEFINITION START*/
		REG_CHIP_ID_ADDR                                     = 0x00,
		REG_ACCEL_REV_ID_ADDR                                = 0x01,
		REG_MAG_REV_ID_ADDR                                  = 0x02,
		REG_GYRO_REV_ID_ADDR                                 = 0x03,
		REG_SW_REV_ID_LSB_ADDR                               = 0x04,
		REG_SW_REV_ID_MSB_ADDR                               = 0x05,
		REG_BL_REV_ID_ADDR                                   = 0X06,

		/* Accel data register */
		REG_ACCEL_DATA_X_LSB_ADDR                            = 0X08,
		REG_ACCEL_DATA_X_MSB_ADDR                            = 0X09,
		REG_ACCEL_DATA_Y_LSB_ADDR                            = 0X0A,
		REG_ACCEL_DATA_Y_MSB_ADDR                            = 0X0B,
		REG_ACCEL_DATA_Z_LSB_ADDR                            = 0X0C,
		REG_ACCEL_DATA_Z_MSB_ADDR                            = 0X0D,

		/* Mag data register */
		REG_MAG_DATA_X_LSB_ADDR                              = 0X0E,
		REG_MAG_DATA_X_MSB_ADDR                              = 0X0F,
		REG_MAG_DATA_Y_LSB_ADDR                              = 0X10,
		REG_MAG_DATA_Y_MSB_ADDR                              = 0X11,
		REG_MAG_DATA_Z_LSB_ADDR                              = 0X12,
		REG_MAG_DATA_Z_MSB_ADDR                              = 0X13,

		/* Gyro data registers */
		REG_GYRO_DATA_X_LSB_ADDR                             = 0X14,
		REG_GYRO_DATA_X_MSB_ADDR                             = 0X15,
		REG_GYRO_DATA_Y_LSB_ADDR                             = 0X16,
		REG_GYRO_DATA_Y_MSB_ADDR                             = 0X17,
		REG_GYRO_DATA_Z_LSB_ADDR                             = 0X18,
		REG_GYRO_DATA_Z_MSB_ADDR                             = 0X19,

		/* Euler data registers */
		REG_EULER_H_LSB_ADDR                                 = 0X1A,
		REG_EULER_H_MSB_ADDR                                 = 0X1B,
		REG_EULER_R_LSB_ADDR                                 = 0X1C,
		REG_EULER_R_MSB_ADDR                                 = 0X1D,
		REG_EULER_P_LSB_ADDR                                 = 0X1E,
		REG_EULER_P_MSB_ADDR                                 = 0X1F,

		/* Quaternion data registers */
		REG_QUATERNION_DATA_W_LSB_ADDR                       = 0X20,
		REG_QUATERNION_DATA_W_MSB_ADDR                       = 0X21,
		REG_QUATERNION_DATA_X_LSB_ADDR                       = 0X22,
		REG_QUATERNION_DATA_X_MSB_ADDR                       = 0X23,
		REG_QUATERNION_DATA_Y_LSB_ADDR                       = 0X24,
		REG_QUATERNION_DATA_Y_MSB_ADDR                       = 0X25,
		REG_QUATERNION_DATA_Z_LSB_ADDR                       = 0X26,
		REG_QUATERNION_DATA_Z_MSB_ADDR                       = 0X27,

		/* Linear acceleration data registers */
		REG_LINEAR_ACCEL_DATA_X_LSB_ADDR                     = 0X28,
		REG_LINEAR_ACCEL_DATA_X_MSB_ADDR                     = 0X29,
		REG_LINEAR_ACCEL_DATA_Y_LSB_ADDR                     = 0X2A,
		REG_LINEAR_ACCEL_DATA_Y_MSB_ADDR                     = 0X2B,
		REG_LINEAR_ACCEL_DATA_Z_LSB_ADDR                     = 0X2C,
		REG_LINEAR_ACCEL_DATA_Z_MSB_ADDR                     = 0X2D,

		/* Gravity data registers */
		REG_GRAVITY_DATA_X_LSB_ADDR                          = 0X2E,
		REG_GRAVITY_DATA_X_MSB_ADDR                          = 0X2F,
		REG_GRAVITY_DATA_Y_LSB_ADDR                          = 0X30,
		REG_GRAVITY_DATA_Y_MSB_ADDR                          = 0X31,
		REG_GRAVITY_DATA_Z_LSB_ADDR                          = 0X32,
		REG_GRAVITY_DATA_Z_MSB_ADDR                          = 0X33,

		/* Temperature data register */
		REG_TEMP_ADDR                                        = 0X34,

		/* Status registers */
		REG_CALIB_STAT_ADDR                                  = 0X35,
		REG_SELFTEST_RESULT_ADDR                             = 0X36,
		REG_INTR_STAT_ADDR                                   = 0X37,

		REG_SYS_CLK_STAT_ADDR                                = 0X38,
		REG_SYS_STAT_ADDR                                    = 0X39,
		REG_SYS_ERR_ADDR                                     = 0X3A,

		/* Unit selection register */
		REG_UNIT_SEL_ADDR                                    = 0X3B,
		REG_DATA_SELECT_ADDR                                 = 0X3C,

		/* Mode registers */
		REG_OPR_MODE_ADDR                                    = 0X3D,
		REG_PWR_MODE_ADDR                                    = 0X3E,

		REG_SYS_TRIGGER_ADDR                                 = 0X3F,
		REG_TEMP_SOURCE_ADDR                                 = 0X40,

		/* Axis remap registers */
		REG_AXIS_MAP_CONFIG_ADDR                             = 0X41,
		REG_AXIS_MAP_SIGN_ADDR                               = 0X42,

		/* SIC registers */
		REG_SIC_MATRIX_0_LSB_ADDR                            = 0X43,
		REG_SIC_MATRIX_0_MSB_ADDR                            = 0X44,
		REG_SIC_MATRIX_1_LSB_ADDR                            = 0X45,
		REG_SIC_MATRIX_1_MSB_ADDR                            = 0X46,
		REG_SIC_MATRIX_2_LSB_ADDR                            = 0X47,
		REG_SIC_MATRIX_2_MSB_ADDR                            = 0X48,
		REG_SIC_MATRIX_3_LSB_ADDR                            = 0X49,
		REG_SIC_MATRIX_3_MSB_ADDR                            = 0X4A,
		REG_SIC_MATRIX_4_LSB_ADDR                            = 0X4B,
		REG_SIC_MATRIX_4_MSB_ADDR                            = 0X4C,
		REG_SIC_MATRIX_5_LSB_ADDR                            = 0X4D,
		REG_SIC_MATRIX_5_MSB_ADDR                            = 0X4E,
		REG_SIC_MATRIX_6_LSB_ADDR                            = 0X4F,
		REG_SIC_MATRIX_6_MSB_ADDR                            = 0X50,
		REG_SIC_MATRIX_7_LSB_ADDR                            = 0X51,
		REG_SIC_MATRIX_7_MSB_ADDR                            = 0X52,
		REG_SIC_MATRIX_8_LSB_ADDR                            = 0X53,
		REG_SIC_MATRIX_8_MSB_ADDR                            = 0X54,

		/* Accelerometer Offset registers */
		REG_ACCEL_OFFSET_X_LSB_ADDR                    	     = 0X55,
		REG_ACCEL_OFFSET_X_MSB_ADDR                          = 0X56,
		REG_ACCEL_OFFSET_Y_LSB_ADDR                          = 0X57,
		REG_ACCEL_OFFSET_Y_MSB_ADDR                          = 0X58,
		REG_ACCEL_OFFSET_Z_LSB_ADDR                          = 0X59,
		REG_ACCEL_OFFSET_Z_MSB_ADDR                          = 0X5A,

		/* Magnetometer Offset registers */
		REG_MAG_OFFSET_X_LSB_ADDR                            = 0X5B,
		REG_MAG_OFFSET_X_MSB_ADDR                            = 0X5C,
		REG_MAG_OFFSET_Y_LSB_ADDR                            = 0X5D,
		REG_MAG_OFFSET_Y_MSB_ADDR                            = 0X5E,
		REG_MAG_OFFSET_Z_LSB_ADDR                            = 0X5F,
		REG_MAG_OFFSET_Z_MSB_ADDR                            = 0X60,

		/* Gyroscope Offset register s*/
		REG_GYRO_OFFSET_X_LSB_ADDR                           = 0X61,
		REG_GYRO_OFFSET_X_MSB_ADDR                           = 0X62,
		REG_GYRO_OFFSET_Y_LSB_ADDR                           = 0X63,
		REG_GYRO_OFFSET_Y_MSB_ADDR                           = 0X64,
		REG_GYRO_OFFSET_Z_LSB_ADDR                           = 0X65,
		REG_GYRO_OFFSET_Z_MSB_ADDR                           = 0X66,

		/* Radius registers */
		REG_ACCEL_RADIUS_LSB_ADDR                            = 0X67,
		REG_ACCEL_RADIUS_MSB_ADDR                            = 0X68,
		REG_MAG_RADIUS_LSB_ADDR                              = 0X69,
		REG_MAG_RADIUS_MSB_ADDR                              = 0X6A
	};

	enum EPowerMode
	{
		POWER_MODE_NORMAL                                    = 0X00,
		POWER_MODE_LOWPOWER                                  = 0X01,
		POWER_MODE_SUSPEND                                   = 0X02
	};

	enum EOpMode : uint8_t
	{
		/* Operation mode settings*/
		OPERATION_MODE_CONFIG                                   = 0X00,
		OPERATION_MODE_ACCONLY                                  = 0X01,
		OPERATION_MODE_MAGONLY                                  = 0X02,
		OPERATION_MODE_GYRONLY                                  = 0X03,
		OPERATION_MODE_ACCMAG                                   = 0X04,
		OPERATION_MODE_ACCGYRO                                  = 0X05,
		OPERATION_MODE_MAGGYRO                                  = 0X06,
		OPERATION_MODE_AMG                                      = 0X07,
		OPERATION_MODE_IMUPLUS                                  = 0X08,
		OPERATION_MODE_COMPASS                                  = 0X09,
		OPERATION_MODE_M4G                                      = 0X0A,
		OPERATION_MODE_NDOF_FMC_OFF                             = 0X0B,
		OPERATION_MODE_NDOF                                     = 0X0C
	};

	// Supported operating modes
	enum class EMode
	{
		GYRO,					// EOpMode::OPERATION_MODE_IMUPLUS
		COMPASS,				// EOpMode::OPERATION_MODE_NDOF
		// COMPASS_SLOW_CAL,	// EOpMode::OPERATION_MODE_NDOF_FMC_OFF

		_MODE_COUNT
	};

	struct TModeInfo
    {
        EOpMode opMode;
        uint8_t updatePeriod;
    };

    // Table of command modifiers and conversion times for each OSR
    constexpr TModeInfo kModeInfo[ (uint8_t)EMode::_MODE_COUNT ] =
    { 
        { EOpMode::OPERATION_MODE_IMUPLUS, 		10 },	// 100Hz
        { EOpMode::OPERATION_MODE_NDOF, 		10 }	// 100Hz
    };

	enum EResult : uint32_t
    {
		// Successes
        RESULT_SUCCESS,
        
		// Errors
		RESULT_ERR_HARD_RESET,
        RESULT_ERR_I2C_TRANSACTION,
		RESULT_ERR_INVALID_MODE,
		RESULT_ERR_INVALID_ID,
		RESULT_ERR_INCORRECT_STATE_AFTER_POR,
		RESULT_ERR_READING_CALIB,
		RESULT_ERR_READING_EULER,
		RESULT_ERR_UNKNOWN,

		// Counter
        _RESULT_COUNT
    };

	enum EState : uint8_t
    {
        POWER_ON_RESET,			// System is initializing from power-on-reset
		USER_RESET,				// System is performing user requested reset
        DISABLED,
        DELAY,
		VERIFYING_CHIP_INFO,
		VERIFYING_CONFIG_MODE,
		CONFIGURING,
		VERIFYING_ACTIVE,
		ACTIVE,

        _STATE_COUNT
    };

	// Delay state info
    struct TTransitionDelay
    {
        orutil::CTimer  timer;
        EState          nextState;
        uint32_t        delayTime_ms;
    };

	struct TRevisionInfo
	{
		uint8_t  accel;
		uint8_t  mag;
		uint8_t  gyro;

		uint8_t  bootloader;
		uint16_t software;

		void Clear()
		{
			accel 		= 0;
			mag 		= 0;
			gyro 		= 0;
			bootloader 	= 0;
			software 	= 0;
		}
	};

	enum EVectorAddress : uint8_t
	{
		VECTOR_ACCELEROMETER = REG_ACCEL_DATA_X_LSB_ADDR,
		VECTOR_MAGNETOMETER  = REG_MAG_DATA_X_LSB_ADDR,
		VECTOR_GYROSCOPE     = REG_GYRO_DATA_X_LSB_ADDR,
		VECTOR_EULER         = REG_EULER_H_LSB_ADDR,
		VECTOR_LINEARACCEL   = REG_LINEAR_ACCEL_DATA_X_LSB_ADDR,
		VECTOR_GRAVITY       = REG_GRAVITY_DATA_X_LSB_ADDR
	};


	enum ESystemStatus : uint8_t
	{
		SYS_STATUS_IDLE 				= 0x00,		// System idle
		SYS_STATUS_ERROR 				= 0x01,		// System error is present. Check SYS_ERR register
		SYS_STATUS_INIT_PERIPHS 		= 0x02,		// Initializing peripherals
		SYS_STATUS_INIT_SYSTEM 			= 0x03,		// Initializing system
		SYS_STATUS_SELFTEST 			= 0x04,		// Executing self-test
		SYS_STATUS_ACTIVE_FUSION 		= 0x05,		// Fusion algorithm running
		SYS_STATUS_ACTIVE_RAW 			= 0x06		// System running without algorithm
	};

	enum ESystemError : uint8_t
	{
		SYS_ERROR_NONE 					= 0x00,		// No error
		SYS_ERROR_PERIPH_INIT 			= 0x01,		// Error initializing peripherals
		SYS_ERROR_SYSTEM_INIT 			= 0x02,		// Error initializing system
		SYS_ERROR_SELFTEST 				= 0x03,		// Self-test failed
		SYS_ERROR_REGMAP_VAL_OOR 		= 0x04,		// Register value out of range
		SYS_ERROR_REGMAP_ADDR_OOR 		= 0x05,		// Register address out of range
		SYS_ERROR_REGMAP_WRITE_ERROR 	= 0x06,		// Register write error
		SYS_ERROR_LOWPWR_UNAVAIL 		= 0x07,		// Low power mode not available for selected operation mode
		SYS_ERROR_ACCEL_PWR_UNAVAIL 	= 0x08,		// Accelerometer power mode not available
		SYS_ERROR_FUSION_CONFIG 		= 0x09,		// Fusion algorithm configuration error
		SYS_ERROR_SENSOR_CONFIG 		= 0x0A		// Sensor configuration error

	};

	enum ECalibration : uint8_t
	{
		CAL_UNCALIBRATED	= 0,
		CAL_PARTIAL			= 1,
		CAL_MOSTLY			= 2,
		CAL_FULLY			= 3
	};

	struct TCalibration
	{
		public:
			ECalibration accel	= ECalibration::CAL_UNCALIBRATED;
			ECalibration gyro	= ECalibration::CAL_UNCALIBRATED;
			ECalibration mag	= ECalibration::CAL_UNCALIBRATED;
			ECalibration system	= ECalibration::CAL_UNCALIBRATED;
		
			bool SampleAvailable()
			{
				if( isAvailable )
				{
					isAvailable = false;
					return true;
				}
				else
				{
					return false;
				}
			}

		private:
			void Update( ECalibration accelIn, ECalibration gyroIn, ECalibration magIn, ECalibration systemIn )
			{
				isAvailable = true;
				accel 		= accelIn;
				gyro 		= gyroIn;
				mag 		= magIn;
				system 		= systemIn;
			}

			bool isAvailable = false;



		friend class BNO055;
	};

	// Data structure
    struct TData
    {
        public:
			float roll		= 0.0f;
			float pitch		= 0.0f;
			float yaw		= 0.0f;
        
            bool SampleAvailable()
            {
                if( isAvailable )
                {
                    isAvailable = false;
                    return true;
                }
                else
                {
                    return false;
                }
            }

        private:
            void Update( float rollIn, float pitchIn, float yawIn )
            {
                isAvailable     = true;
				roll 			= rollIn;
				pitch 			= pitchIn;
				yaw 			= yawIn;
            }

            bool isAvailable = false;


        friend class BNO055;
    };

}