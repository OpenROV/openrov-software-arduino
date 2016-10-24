#pragma once

// Includes
#include <Arduino.h>
#include "CTimer.h"

// Data structures
struct TNavData
{
	double HDGD;	// Compass heading in degrees

	float DEEP;		// Depth in meters
	float ROLL;		// Roll in degrees
	float PITC;		// Pitch in degrees
	float YAW;		// Yaw in degrees
	float FTHR;		// % of power in forward thrust
};

struct TEnvironmentData
{
	float PRES;		// Pressure in millibars
	float TEMP;		// Temperature in C
};

struct TCapeData
{
	uint32_t FMEM;	// Free memory on the Arduino in bytes
	double VOUT;	// Voltage as meassed at the cape in milli-volts
	double IOUT;	// Current measured in to the cape in milli-amps.
	double BTTI;	// Current draw from both battery banks
	uint32_t UTIM;	// Up-time since Arduino was started in milliseconds
};

struct TThrusterData
{
	bool MATC;			// Motors on-line indicator
	bool MotorsActive;	// Whether or not there is currently a motor running
};

struct TControllerData
{
	float roll;
	float pitch;
	float yaw;
	float depth;
	float forwardSpeed;

	float rollSetpoint;
	float pitchSetpoint;
	float yawSetpoint;
	float depthSetpoint;
	float forwardSpeedSetpoint;

	float rollCommand;
	float pitchCommand;
	float yawCommand;
	float depthCommand;
	float forwardSpeedCommand;

	float yawError;
};

// Shared data namespace
namespace NDataManager
{
	extern TNavData				m_navData;
	extern TEnvironmentData		m_environmentData;
	extern TCapeData			m_capeData;
	extern TThrusterData		m_thrusterData;
	extern TControllerData		m_controllerData;

	extern CTimer				m_timer_1hz;
	extern CTimer				m_timer_10hz;

	extern uint32_t				m_loopsPerSec;

	extern void Initialize();
	extern void OutputSharedData();
	extern void OutputNavData();
	extern void HandleOutputLoops();
}