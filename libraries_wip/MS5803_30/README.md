Arduino library for the Measurement Specialties MS5803-30BA pressure sensor modules. This library
DOES NOT work with other pressure range modules like the MS5803-14BA, and it will return incorrect
pressure and temperature values if used with other models. See http://github.com/millerlp for 
libraries to use with the other MS5803 pressure sensor models.

* MS5803-01BA: http://github.com/millerlp/MS5803_01 
* MS5803-02BA: http://github.com/millerlp/MS5803_02 
* MS5803-05BA: http://github.com/millerlp/MS5803_05 
* MS5803-14BA: http://github.com/millerlp/MS5803_14 
* MS5803-30BA: http://github.com/millerlp/MS5803_30 

The MS5803 pressure sensor works on voltages around 3 volts. To use it with a 5V Arduino,
you need to supply the sensor power from the Arduino's 3V3 voltage output. Additionally,
you must place 10k ohm resistors between the SDA + SCL I2C communication lines and the 3V3 
voltage supply to keep the data lines from exceeding 3.3V. The manufacturer recommends placing 
a 0.1 microFarad (100 nF) ceramic capacitor between pin 5 (Vdd) and ground. 

This library assumes the MS5803 is set to the I2C address 0x76, created by wiring CSB (pad 3)
to the 3.3V supply. In addition, PS (pad 6) must be tied to 3.3V supply to invoke I2C 
communications mode. The SPI protocol is not supported in this library because I'm lazy. 

 

The start of your Arduino sketch must include the lines:


```
#include <Wire.h>
#include "MS5803_30.h"

MS_5803 sensor = MS_5803(512);

```

to use this library. The function MS_5803() takes one argument for the oversampling range. Available
values are 256, 512, 1024, 2048, and 4096. The default is 512 if you do not enter a 
value manually. Larger values include longer delays to allow the sensor to complete
a sample. 

In the setup loop, initialize the sensor as follows:
```
	// This must be in the setup loop. arguments: true or false for verbose output
	// Returns a boolean true or false depending on whether the CRC error check
	// succeeds or fails. See the example sketch.
	sensor.initializeMS_5803(true) 
```

Other useful commands:
```

	readSensor() // Get temperature and pressure from sensor

	temperature() // Get temperature in Celsius (returns a float value)
	
	pressure() // Get pressure in mbar (returns a float value)
```
