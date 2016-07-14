#pragma once

// Includes
#include <Arduino.h>
#include "SystemConstants.h"

struct TInternalCommand
{
	char text[ COMMAND_DATA_BUFFER_SIZE + 1 ]; // Add 1 for NULL terminator
	int arguments[ COMMAND_MAX_ARGUMENTS ];
};

class CCommand
{
	// Attributes
public:
	static int m_arguments[ COMMAND_MAX_ARGUMENTS ];

private:
	static char m_text[ COMMAND_DATA_BUFFER_SIZE + 1 ];

	// Methods
public:
	boolean GetCommandString();
	boolean Equals( const char* commandStringIn );

	static void PushCommand( char* textIn, int argumentsIn[ COMMAND_MAX_ARGUMENTS ] );
	static void Reset();

private:
	void Parse();
};
