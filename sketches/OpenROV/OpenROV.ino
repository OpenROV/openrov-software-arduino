#include <Arduino.h>

void setup()
{
	//digitalWrite(PIN_LED_0, HIGH);
	//digitalWrite(PIN_LED_1, HIGH);

	//Serial.begin( 115200 );
	//Serial1.begin( 115200 );
}

void loop()
{
	digitalWrite(PIN_LED, HIGH);   // turn the LED on (HIGH is the voltage level)
  	delay(1000);              // wait for a second
  	digitalWrite(PIN_LED, LOW);    // turn the LED off by making the voltage LOW
 	delay(1000);

	//Serial.println( "Hello0" );
	//Serial1.println( "Hello1" );   
}
