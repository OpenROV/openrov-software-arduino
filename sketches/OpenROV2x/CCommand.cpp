#include "CCommand.h"

// File local variables and methods
namespace
{
	char				dataBuffer[COMMAND_DATA_BUFFER_SIZE + 1]; //Add 1 for NULL terminator
	uint8_t				dataBufferIndex = 0;
	bool				commandReady = false;
	const char			endChar = ';'; // or '!', or whatever your end character is
	bool				storeString = false; //This will be our flag to put the data in our buffer
	TInternalCommand	internalCommandBuffer[ COMMAND_MAX_COUNT ];
	int					internalCommandBuffer_head = 0;
	int					internalCommandBuffer_tail = 0;

	// CRC-8 - based on the CRC8 formulas by Dallas/Maxim
	// Code released under the therms of the GNU GPL 3.0 license
	uint8_t CRC8( uint8_t start, char* data, uint8_t len )
	{
		uint8_t crc = 0x00;

		for( uint8_t j = 0; j < start; j++ )
		{
			*data++;
		}

		while( len-- )
		{
			uint8_t extract = *data++;

			for( uint8_t tempI = 8; tempI; tempI-- )
			{
				uint8_t sum = ( crc ^ extract ) & 0x01;
				crc >>= 1;

				if( sum )
				{
					crc ^= 0x8C;
				}

				extract >>= 1;
			}
		}

		return crc;
	}


	bool GetSerialString()
	{
		while( Serial.available() > 0 )
		{
			char incomingbyte = Serial.read();

			// Start accumulating new string
			if( storeString == false )
			{
				storeString = true;
				dataBufferIndex = 0;
			}

			// If accumulating
			if( storeString )
			{
				//Let's check our index here, and abort if we're outside our buffer size
				//We use our define here so our buffer size can be easily modified
				if( dataBufferIndex == COMMAND_DATA_BUFFER_SIZE )
				{
					//Oops, our index is pointing to an array element outside our buffer.
					dataBufferIndex = 0;
					break;
				}

				if( incomingbyte == endChar )
				{
					dataBuffer[dataBufferIndex++] = incomingbyte;
					dataBuffer[dataBufferIndex] = 0; //null terminate the C string
					storeString = false;

					//Our data string is complete.  return true
					return true;
				}
				else
				{
					dataBuffer[dataBufferIndex++] = incomingbyte;
					dataBuffer[dataBufferIndex] = 0; //null terminate the C string
				}
			}
			else
			{
			}
		}

		//We've read in all the available Serial data, and don't have a valid string yet, so return false
		return false;
	}
}


// Static member initialization
int32_t CCommand::m_arguments[COMMAND_MAX_ARGUMENTS];
char CCommand::m_text[COMMAND_DATA_BUFFER_SIZE + 1] ;

bool CCommand::Equals( const char* targetcommand )
{
	if( !commandReady )
	{
		return false;
	}

	char* pos = strstr( m_text, targetcommand );

	if( pos == m_text ) //starts with
	{
		return true;
	}

	return false;
}


bool CCommand::GetCommandString()
{
	// Get string from buffer

	commandReady = false;
	strcpy( m_text, "" );

	for( int i = 0; i < COMMAND_MAX_ARGUMENTS; i++ )
	{
		m_arguments[i] = 0;
	}

	if( GetSerialString() )
	{
		//String available for parsing.  Parse it here
		Parse();
		commandReady = true;
		return true;
	}

	if( internalCommandBuffer_head != internalCommandBuffer_tail )
	{
		// Advance tail index
		internalCommandBuffer_tail++;

		// Wrap around
		if( internalCommandBuffer_tail == COMMAND_MAX_COUNT )
		{
			internalCommandBuffer_tail = 0;
		}

		// Get from the command buffer
		TInternalCommand c = internalCommandBuffer[ internalCommandBuffer_tail ];

		// Copy command text
		strcpy( m_text, c.text );

		// Check for invalid command
		if( strcmp( m_text, "" ) == 0 )
		{
			Serial.print( F( "icmd: CMD MUNGED!;" ) );
			return false;
		}

		// Print command
		Serial.print( F( "icmd:" ) );
		Serial.print( m_text );
		Serial.print( '(' );

		for( int i = 1; i < c.arguments[0] + 1; i++ )
		{
			m_arguments[i] = c.arguments[i];

			if( i > 1 )
			{
				Serial.print( ',' );
			}

			Serial.print( m_arguments[i] );
		}

		m_arguments[0] = c.arguments[0];
		//need to add the trailing # of arguments to the count or else have people do it in the call which sucks.

		commandReady = true;

		Serial.println( ");" );
		return true;
	}

	return false;
}

void CCommand::PushCommand( const char* cmdtext, int32_t cmdargs[COMMAND_MAX_ARGUMENTS] )
{
	// If commands are not being processed in time we overwrite the oldest ones.  Technically we should probably
	// have a global array for all possible commands where only the most recent is ever processed to prevent
	// stale messages from floating around.
	TInternalCommand c;
	strcpy( c.text, cmdtext );

	if( strlen( c.text ) < 1 )
	{
		Serial.print( F( "pushcmd: cmdtext MUNGED!;" ) );
	}

	for( int i = 0; i < cmdargs[0] + 1; i++ )
	{
		c.arguments[i] = cmdargs[i];
	}

	internalCommandBuffer_head++;

	if( internalCommandBuffer_head == COMMAND_MAX_COUNT )
	{
		internalCommandBuffer_head = 0;
	}

	//go ahead and drop the command that has not yet been executed
	if( internalCommandBuffer_head == internalCommandBuffer_tail )
	{
		Serial.println( F( "log: CommandBufferOverrun;" ) );
		internalCommandBuffer_tail++;
	}

	if( internalCommandBuffer_tail == COMMAND_MAX_COUNT )
	{
		internalCommandBuffer_tail = 0;
	}

	internalCommandBuffer[internalCommandBuffer_head] = c;
}


void CCommand::Reset()
{
	// Removes any state
	commandReady				= false;
	storeString					= false;
	dataBufferIndex				= 0;
	internalCommandBuffer_head	= 0;
	internalCommandBuffer_tail	= 0;
}

// get 'arguments' from command
void CCommand::Parse()
{
	char* pch;
	uint8_t crc = 0;
	uint8_t i = 0;
	crc = CRC8( 1, dataBuffer, dataBufferIndex - 1 );

	Serial.print( F( "rawcmd:" ) );
	uint8_t tt = 0;

	while( tt < dataBufferIndex )
	{
		uint8_t zz = dataBuffer[tt];
		Serial.print( zz, HEX );
		Serial.print( ',' );
		tt++;
	}

	Serial.println( ';' );

	Serial.print( F( "crc:" ) );
	uint8_t testcrc = dataBuffer[0];

	if( crc == testcrc )
	{
		Serial.print( F( "pass;" ) );
	}
	else
	{
		Serial.print( F( "fail," ) );
		Serial.print( crc, HEX );
		Serial.print( "/" );
		Serial.print( testcrc, HEX );
		Serial.print( ';' );
		return;
	}

	char* db2 = dataBuffer;
	db2++;
	pch = strtok( db2, " ,();" );

	while( pch != NULL )
	{
		if( i == 0 )
		{
			//this is the command text
			Serial.print( F( "cmd:" ) );
			Serial.print( pch );
			Serial.print( '(' );
			strcpy( m_text, pch );
		}
		else
		{
			//this is a parameter
			m_arguments[i] = atol( pch );

			if( i > 1 )
			{
				Serial.print( ',' );
			}

			Serial.print( pch );
		}

		i++;
		pch = strtok( NULL, " ,();" );
	}

	Serial.println( ");" );
	m_arguments[0] = i - 1;
}
