// Includes
#include <Arduino.h>

#include "CModule.h"
#include "NModuleManager.h"

CModule::CModule()
	: m_name( "NA" )
	, m_executionTime( 0 )
{
	NModuleManager::RegisterModule( this );
}