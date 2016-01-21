// Includes
#include <Arduino.h>

#include "CModule.h"
#include "NModuleManager.h"

CModule::CModule()
	: m_name( "NA" )
	, m_executionTime( 0 )
{
	// Serial.println( "LOL" );
	NModuleManager::RegisterModule( this );
	// Serial.println( "NO" );
}