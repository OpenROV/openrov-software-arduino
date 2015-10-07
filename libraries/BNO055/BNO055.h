/***************************************************************************
  This is a library for the BNO055 orientation sensor

  Designed specifically to work with the Adafruit BNO055 Breakout.

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products

  These sensors use I2C to communicate, 2 pins are required to interface.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!

  Written by KTOWN for Adafruit Industries.

  MIT license, all text above must be included in any redistribution
 ***************************************************************************/
 
 // Modified for use in OpenROV's Software

#pragma once

#include "LibBNO055_Definitions.h"
#include "LibBNO055_imumaths.h"

#define BNO055_ADDRESS_A (0x28)
#define BNO055_ADDRESS_B (0x29)
#define BNO055_ID        (0xA0)

// Forward declaration
class TwoWire;

class CBNO055Driver
{
public:

	CBNO055Driver( int32_t sensorIdIn = -1, uint8_t addressIn = BNO055_ADDRESS_A );

	bool Initialize( TwoWire *wireInterfaceIn );
	bool Reset();
	bool SetMode( bosch::EOpMode modeIn );
	bool SetExternalCrystalUse( boolean shouldUseIn );
	bool SetPowerMode( bosch::EPowerMode powerModeIn );
	bool SetSelectedRegisterPage( byte pageIdIn );
	bool SetUpUnitsAndOrientation();
	bool VerifyChipId();
	bool GetCalibration();
	bool GetRevInfo( bosch::TRevisionInfo& revInfoOut );
	bool GetVector( bosch::EVectorType vectorTypeIn, imu::Vector<3>& vectorOut );
	bool GetQuat( imu::Quaternion& quatOut );
	bool GetTemperature( int8_t& temperatureOut );
	bool GetPowerOnSelfTestResults();
	bool GetSystemStatus();
	bool GetSystemError();
	bool GetSoftwareVersion();
	bool GetBootloaderRev();
	bool GetOperatingMode();

	bool GetAccelerometerOffsets();
	bool GetGyroOffsets();
	bool GetMagnetometerOffsets();

	bool EnterIMUMode();
	bool EnterNDOFMode();

	// CAdafruitSensor implementation

	bool m_isInitialized;

	uint8_t m_systemCal;
	uint8_t m_gyroCal;
	uint8_t m_magCal;
	uint8_t m_accelCal;

	uint8_t m_systemStatus;
	uint8_t m_powerOnSelfTestResult;
	uint8_t m_biltSelfTestResult;
	uint8_t m_systemError;

	uint8_t m_softwareVersionMinor;
	uint8_t m_softwareVersionMajor;
	uint8_t m_bootloaderRev;

	uint8_t m_operatingMode;

private:
	bool ReadByte( bosch::ERegisterAddress addressIn, uint8_t& dataOut );
	bool ReadNBytes( bosch::ERegisterAddress addressIn, uint8_t* dataOut, uint8_t byteCountIn );
	bool WriteByte( bosch::ERegisterAddress addressIn, uint8_t dataIn );

	void ClearWire();

	bool m_lastRetcode;

	static const unsigned long	k_configToOpDelay_ms	= 30;
	static const unsigned long	k_opToConfigDelay_ms	= 30;
	static const unsigned long	k_resetDelay_ms			= 1000;
	static const unsigned long	k_selfTestDelay_ms		= 1000;

	uint8_t		m_i2cAddress;
	int32_t		m_sensorId;
	bosch::EOpMode		m_mode;

	bosch::TCalibrationOffsets m_accOffsets;
	bosch::TCalibrationOffsets m_gyroOffsets;
	bosch::TCalibrationOffsets m_magOffsets;
	
	TwoWire *m_pWire;
};
