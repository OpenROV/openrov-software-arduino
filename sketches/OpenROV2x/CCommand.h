#pragma once

// Includes
#include <Arduino.h>

// Commands
#define COMMAND_MAX_ARGUMENTS			10
#define COMMAND_MAX_STRING_LENGTH		40
#define COMMAND_MAX_COUNT				5	// Use 3 for 328p
#define COMMAND_DATA_BUFFER_SIZE		80

struct TInternalCommand
{
	char text[ COMMAND_DATA_BUFFER_SIZE + 1 ]; // Add 1 for NULL terminator
	int32_t arguments[ COMMAND_MAX_ARGUMENTS ];
};

class CCommand
{
	// Attributes
public:
	static int32_t m_arguments[ COMMAND_MAX_ARGUMENTS ];

private:
	static char m_text[ COMMAND_DATA_BUFFER_SIZE + 1 ];

	// Methods
public:
	bool GetCommandString();
	bool Equals( const char* commandStringIn );

	static void PushCommand( const char* textIn, int32_t argumentsIn[ COMMAND_MAX_ARGUMENTS ] );
	static void Reset();

private:
	void Parse();
};
