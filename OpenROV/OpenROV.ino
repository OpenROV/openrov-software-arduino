#if __cplusplus <= 199711L
//#error Must set Arduino GCC compiler C++ version to c++11
#endif

// Includes
#include <Wire.h>
#include <EEPROM.h>
#include <SPI.h>

#include "NCommManager.h"
#include "NModuleManager.h"
#include "NArduinoManager.h"
#include "NDataManager.h"
#include "NConfigManager.h"

void setup()
{
    // Initialize main subsystems
    NArduinoManager::Initialize();
    NCommManager::Initialize();
    NConfigManager::Initialize();
    NModuleManager::Initialize();
    NDataManager::Initialize();

    // Boot complete
    Serial.println( F( "boot:1;" ) );
}

void loop()
{
    // Reset the watchdog timer
    wdt_reset();

    // Attempt to read a current command off of the command line
    NCommManager::GetCurrentCommand();

    // Handle any config change requests
    NConfigManager::HandleMessages( NCommManager::m_currentCommand );

    // Handle update loops for each module
    NModuleManager::HandleModuleUpdates( NCommManager::m_currentCommand );

    // Handle update loops that send data back to the beaglebone
    NDataManager::HandleOutputLoops();
}
