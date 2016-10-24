// Includes
#include <Arduino.h>

#include "CModule.h"
#include "NModuleManager.h"

CModule::CModule()
{
	NModuleManager::RegisterModule( this );
}