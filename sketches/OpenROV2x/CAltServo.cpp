#include "SysConfig.h"
#if(HAS_ALT_SERVO)

#include "CPin.h"
#include "CAltServo.h"
#include <Servo.h>
#include "PinDefinitions.h"

// AltServo
#define ALTS_MIDPOINT 1500
#define ALTS_MINPOINT 1000
#define ALTS_MAXPOINT 2000

namespace
{
	Servo _altservo;

	int alts_val = ALTS_MIDPOINT;
	int new_alts = ALTS_MIDPOINT;
	int altsrate = 1;

	int smoothAdjustedServo( int target, int current )
	{
		double x = target - current;
		int sign = ( x > 0 ) - ( x < 0 );
		int adjustedVal = current + sign * ( min( abs( target - current ), altsrate ) );
		return ( adjustedVal );
	}
}

void CAltServo::Initialize()
{
	_altservo.attach( PIN_ALTSERVO );
	_altservo.writeMicroseconds( ALTS_MIDPOINT );
}

void CAltServo::Update( CCommand& commandIn )
{
	if( commandIn.Equals( "asrt" ) )
	{
		int ms = map( commandIn.m_arguments[1], -100, 100, ALTS_MINPOINT, ALTS_MAXPOINT );

		if( ( ms >= ALTS_MINPOINT ) && ( ms <= ALTS_MAXPOINT ) )
		{
			alts_val = ms;
			Serial.print( F( "asr.t:" ) );
			Serial.print( alts_val );
			Serial.println( ';' );
		}
	}

	if( alts_val != new_alts )
	{
		new_alts = smoothAdjustedServo( alts_val, new_alts );
		_altservo.writeMicroseconds( new_alts );
		Serial.print( F( "asr.v:" ) );
		Serial.print( new_alts );
		Serial.println( ';' );
	}


}

#endif
