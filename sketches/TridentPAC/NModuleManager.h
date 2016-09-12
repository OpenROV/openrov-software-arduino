#pragma once

// Forward declarations
class CModule;

namespace NModuleManager
{
	// Public Methods
	extern void Initialize();
	extern void InstallModule( CModule *moduleIn );
	extern void HandleModuleUpdates();
}