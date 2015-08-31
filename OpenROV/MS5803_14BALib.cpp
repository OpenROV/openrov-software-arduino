#include "MS5803_14BALib.h"
#include <Arduino.h>

/* It appears that the gen 2 of the MS5803 series is embedding
   the pow(2, X) used in the final calculation that is unique
   to the sensor in the vendor reserved space.  8192 & CalConstant[0]
   seems to mostly = 8192.  Not reliable enough to pull yet.  But
   all of the new sensors do have values in the CalConstant[0] whereas
   the 14bar sensor we were using does not, so I will use that to auto
   detect which sensor is used at the moment
*/
static unsigned int MS5803_Model_Varient(unsigned int CalConstant[8]){
  if (CalConstant[0]!=0) return 8192; // (2^13)
  return 32768;// (2^15)
}

float CorrectedTemperature(long AdcTemperature, unsigned int CalConstant[8]){
  float T2,Off2,Sens2,TempDifference,Temperature;
  // Calculate the Temperature (first-order computation)

  TempDifference = (float)(AdcTemperature - ((long)CalConstant[5] << 8));
  Temperature = (TempDifference * (float)CalConstant[6])/ pow(2, 23);
  Temperature = Temperature + 2000;  // This is the temperature in hundredths of a degree C

  // Calculate the second-order offsets

  if (Temperature < 2000.0)  // Is temperature below or above 20.00 deg C ?
  {
    T2 = 3 * pow(TempDifference, 2) / pow(2, 33);
    Off2 = 1.5 * pow((Temperature - 2000.0), 2);
    Sens2 = 0.625 * pow((Temperature - 2000.0), 2);
  }
  else
  {
    T2 = (TempDifference * TempDifference) * 7 / pow(2, 37);
    Off2 = 0.0625 * pow((Temperature - 2000.0), 2);
    Sens2 = 0.0;
  }

  // Print the temperature results

  Temperature = Temperature / 100;  // Convert to degrees C
  // Calculate the pressure parameters
  return Temperature - (T2 / 100);
}

float TemperatureCorrectedPressure(long AdcPressure, long AdcTemperature, unsigned int CalConstant[8] ){
  return TemperatureCorrectedPressure_2(AdcPressure, AdcTemperature, CalConstant);
}


float TemperatureCorrectedPressure_1(long AdcPressure, long AdcTemperature, unsigned int CalConstant[8] ){
   float T2,Off2,Sens2,TempDifference,Temperature;
  // Calculate the Temperature (first-order computation)

  TempDifference = (float)(AdcTemperature - ((long)CalConstant[5] << 8));
  Temperature = (TempDifference * (float)CalConstant[6])/ pow(2, 23);
  Temperature = Temperature + 2000;  // This is the temperature in hundredths of a degree C

  // Calculate the second-order offsets

  if (Temperature < 2000.0)  // Is temperature below or above 20.00 deg C ?
  {
    T2 = 3 * pow(TempDifference, 2) / pow(2, 33);
    Off2 = 1.5 * pow((Temperature - 2000.0), 2);
    Sens2 = 0.625 * pow((Temperature - 2000.0), 2);
  }
  else
  {
    T2 = (TempDifference * TempDifference) * 7 / pow(2, 37);
    Off2 = 0.0625 * pow((Temperature - 2000.0), 2);
    Sens2 = 0.0;
  }

  float Offset, Sensitivity, Pressure;

  TempDifference = (float)(AdcTemperature - ((long)CalConstant[5] << 8));

  Offset = (float)CalConstant[2] * pow(2,16);
  Offset = Offset + ((float)CalConstant[4] * TempDifference / pow(2, 7));

  Sensitivity = (float)CalConstant[1] * pow(2, 15);
  Sensitivity = Sensitivity + ((float)CalConstant[3] * TempDifference / pow(2, 8));

  // Add second-order corrections

  Offset = Offset - Off2;
  Sensitivity = Sensitivity - Sens2;

  // Calculate absolute pressure in bars

  Pressure = (float)AdcPressure * Sensitivity / pow(2, 21);
  Pressure = Pressure - Offset;
  Pressure = Pressure / pow(2, 15);
  Pressure = Pressure / 10;  // Set output to mbars = hectopascal;
  return Pressure;
}

float TemperatureCorrectedPressure_2(long AdcPressure, long AdcTemperature, unsigned int CalConstant[8] ){
  uint32_t     D1 = AdcPressure;    // Store uncompensated pressure value
  uint32_t     D2 = AdcTemperature;    // Store uncompensated temperature value
  // These three variables are used for the conversion steps
  // They should be signed 32-bit integer initially
  // i.e. signed long from -2147483648 to 2147483647
  int32_t	dT = 0;
  int32_t 	TEMP = 0;
  // These values need to be signed 64 bit integers
 // (long long = int64_t)
  int64_t	Offset = 0;
  int64_t	Sensitivity  = 0;
  int64_t	T2 = 0;
  int64_t	OFF2 = 0;
  int64_t	Sens2 = 0;
  // Some constants used in calculations below
  const uint64_t POW_2_33 = 8589934592ULL; // 2^33 = 8589934592
  const uint64_t POW_2_37 = 137438953472ULL; // 2^37 = 137438953472
  float mbar; // Store pressure in mbar.
  float tempC; // Store temperature in degrees Celsius
  int32_t mbarInt; // pressure in mbar, initially as a signed long integer

  dT = (int32_t)D2 - ( (int32_t)CalConstant[5] * 256 );
  // Use integer division to calculate TEMP. It is necessary to cast
  // one of the operands as a signed 64-bit integer (int64_t) so there's no
  // rollover issues in the numerator.
  TEMP = 2000 + ((int64_t)dT * CalConstant[6]) / 8388608LL;
  // Recast TEMP as a signed 32-bit integer
  TEMP = (int32_t)TEMP;


  // All operations from here down are done as integer math until we make
  // the final calculation of pressure in mbar.


  // Do 2nd order temperature compensation (see pg 9 of MS5803 data sheet)
  // I have tried to insert the fixed values wherever possible
  // (i.e. 2^31 is hard coded as 2147483648).
  if (TEMP < 2000) {
  // For 30 bar model
  T2 = 3 * ((int64_t)dT * dT) / POW_2_33 ;
  T2 = (int32_t)T2; // recast as signed 32bit integer
  OFF2 = 3 * ((TEMP-2000) * (TEMP-2000)) / 2 ;
  Sens2 = 5 * ((TEMP-2000) * (TEMP-2000)) / 8 ;
  } else { // if TEMP is > 2000 (20.0C)
    // For 30 bar model
  T2 = 7 * ((int64_t)dT * dT) / POW_2_37;
  T2 = (int32_t)T2; // recast as signed 32bit integer
  OFF2 = 1 * ((TEMP-2000) * (TEMP-2000)) / 16;
  Sens2 = 0;
  }
  // Additional compensation for very low temperatures (< -15C)
  if (TEMP < -1500) {
    // For 30 bar model
  OFF2 = OFF2 + 7 * ((TEMP+1500)*(TEMP+1500));
  Sens2 = Sens2 + 4 * ((TEMP+1500)*(TEMP+1500));
  }

  // Calculate initial Offset and Sensitivity
  // Notice lots of casts to int64_t to ensure that the
  // multiplication operations don't overflow the original 16 bit and 32 bit
  // integers
// For 30 bar sensor
Offset = (int64_t)CalConstant[2] * 65536 + (CalConstant[4] * (int64_t)dT) / 128;
Sensitivity = (int64_t)CalConstant[1] * 32768 + (CalConstant[3] * (int64_t)dT) / 256;

  // Adjust TEMP, Offset, Sensitivity values based on the 2nd order
  // temperature correction above.
  TEMP = TEMP - T2; // both should be int32_t
  Offset = Offset - OFF2; // both should be int64_t
  Sensitivity = Sensitivity - Sens2; // both should be int64_t

  // Final compensated pressure calculation. We first calculate the pressure
  // as a signed 32-bit integer (mbarInt), then convert that value to a
  // float (mbar).
  // For 30 bar sensor
  mbarInt = ((D1 * Sensitivity) / 2097152 - Offset) / MS5803_Model_Varient(CalConstant);

//  Serial.print("D1:");Serial.println(D1);
//  Serial.print("D2:");Serial.println(D2);
//  Serial.print("Sensitivity:");Serial.println(Sensitivity);
//  Serial.print("Offset:");Serial.println(Offset);
  mbar = (float)mbarInt / 10;

  // Calculate the human-readable temperature in Celsius
  tempC  = (float)TEMP / 100;


  //    // Start other temperature conversions by converting mbar to psi absolute
  //    psiAbs = mbar * 0.0145038;
  //    // Convert psi absolute to inches of mercury
  //    inHgPress = psiAbs * 2.03625;
  //    // Convert psi absolute to gauge pressure
  //    psiGauge = psiAbs - 14.7;
  //    // Convert mbar to mm Hg
  //    mmHgPress = mbar * 0.7500617;
  //    // Convert temperature to Fahrenheit
  //    tempF = (tempC * 1.8) + 32;
  return mbar;

}

