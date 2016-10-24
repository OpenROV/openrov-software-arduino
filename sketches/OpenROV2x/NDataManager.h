#pragma once

// Includes
#include <Arduino.h>
#include <orutil.h>

// Data structures
struct TNavData
{
    float DEEP  = 0.0f;		// Depth in meters
    float ROLL  = 0.0f;		// Roll in degrees
    float PITC  = 0.0f;		// Pitch in degrees
    float YAW   = 0.0f;		// Yaw in degrees
    float FTHR  = 0.0f;		// % of power in forward thrust
};

struct TEnvironmentData
{
    float PRES  = 0.0f;		// Pressure in millibars
    float TEMP  = 0.0f;		// Temperature in C
};

struct TCapeData
{
    uint32_t FMEM   = 0;	// Free memory on the Arduino in bytes
    float VOUT      = 0.0f;	// Voltage as meassed at the cape in milli-volts
    float IOUT      = 0.0f;	// Current measured in to the cape in milli-amps.
    float BTTI      = 0.0f;	// Current draw from both battery banks
    uint32_t UTIM   = 0;	// Up-time since Arduino was started in milliseconds
};

struct TThrusterData
{
    bool MATC           = false;    // Motors on-line indicator
    bool MotorsActive   = false;	// Whether or not there is currently a motor running
};

// Shared data namespace
namespace NDataManager
{
    extern TNavData				m_navData;
    extern TEnvironmentData		m_environmentData;
    extern TCapeData			m_capeData;
    extern TThrusterData		m_thrusterData;

    extern orutil::CTimer		m_timer_1hz;
    extern orutil::CTimer	    m_timer_10hz;

    extern uint32_t				m_loopsPerSec;

    extern void Initialize();
    extern void OutputSharedData();
    extern void OutputNavData();
    extern void HandleOutputLoops();
}