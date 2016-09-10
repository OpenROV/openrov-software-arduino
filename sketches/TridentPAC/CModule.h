#pragma once

#include "CCommand.h"
#include <Arduino.h>

class CModule
{
public:
	enum class ERegistrationStatus : uint8_t
	{	
		UNREGISTERED,	// Module has not been acked by Cockpit yet
		REGISTERED,		// Module has been acked and registered with Cockpit
		DISABLING,		// Module is currently in the process of disabling
		DISABLED		// Module has removed itself from the registration process
	}

	enum class EModuleStatus : uint8_t
	{	
		UNINITIALIZED,	// Init has not yet been called for the module
		ACTIVE,			// Module is registered, initialized, and active
		DISABLED		// Module has been disabled permanently due to too many faults
	}

	// Attributes
	const uint32_t		m_pid;
	bool 				m_hasUUID;
	uint32_t			m_uuid;
	const char*			m_behaviors;
	const char*			m_traits;

	ERegistrationStatus	m_regStatus;
	EModuleStatus		m_modStatus;

	CModule( const char* behaviorsIn, const char* traitsIn );

	void HandleRegistration();
	void HandleInitialization();
	void HandleUpdate();

	void SetUUID( uint32_t uuidIn );											// Called by NModuleManager to assign a new UUID

	void ResetModule();															// Allows a supervisor to soft reset the module back to: 			m_modStatus = UNINITIALIZED
	void DisableModule();														// Allows us to tell cockpit the module is no longer available: 	m_modStatus = DISABLED

	void UpdateRegistration( const char* behaviorsIn, const char* traitsIn );	// Allows the module to change modalities and inform cockpit: 		m_hasUUID = false, m_regStatus = UNREGISTERED
	void DisableRegistration();													// Allows module to remove itself from the registration process: 	m_regStatus = DISABLED

	// Pure virtuals - These methods must be instantiated in the module's implementation
	virtual void Initialize() = 0;						
	virtual void Update( CCommand &commandIn ) = 0;
};