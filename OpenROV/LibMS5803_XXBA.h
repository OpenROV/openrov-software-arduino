#ifndef __MS5803_14BALIB_H_
#define __MS5803_14BALIB_H_


float CorrectedTemperature(long AdcTemperature, unsigned int CalConstant[8]);

float TemperatureCorrectedPressure(long AdcPressure, long AdcTemperature, unsigned int CalConstant[8] );


float TemperatureCorrectedPressure_1(long AdcPressure, long AdcTemperature, unsigned int CalConstant[8] );

float TemperatureCorrectedPressure_2(long AdcPressure, long AdcTemperature, unsigned int CalConstant[8] );

#endif

