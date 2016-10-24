////////////////////////////////////////////////////////////////////////////
//
//  This file is part of MPU9150
//
//  Copyright (c) 2013 Pansenti, LLC
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy of
//  this software and associated documentation files (the "Software"), to deal in
//  the Software without restriction, including without limitation the rights to use,
//  copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
//  Software, and to permit persons to whom the Software is furnished to do so,
//  subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in all
//  copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
//  INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
//  PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
//  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
//  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
//  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef _MPU9150_H_
#define _MPU9150_H_

#include "MPU9150_Def.h"
#include "MPU9150_Quaternion.h"
#include "MPU9150_Calibration.h"

//  This symbol defines the scaled range for mag and accel values
#define SENSOR_RANGE   4096

namespace mpu9150
{
	class MPU9150
	{
	public:
		// Attributes
		long 	m_rawQuaternion[4];				// the quaternion output from the DMP
		short 	m_rawGyro[3];					// calibrated gyro output from the sensor
		short 	m_rawAccel[3];					// raw accel data
		short 	m_rawMag[3];					// raw mag data

		float 	m_dmpQuaternion[4];				// float and normalized version of the dmp quaternion
		float 	m_dmpEulerPose[3];				// Euler angles from the DMP quaternion
		short 	m_calAccel[3];					// calibrated and scaled accel data
		short 	m_calMag[3];					// calibrated mag data

		float 	m_fusedEulerPose[3];			// the fused Euler angles
		float 	m_fusedQuaternion[4];			// the fused quaternion

		// Constructor
		MPU9150( EAddress addressIn );
		
		void useAccelCal( bool useCal );
		void useMagCal( bool useCal );

		// init must be called to setup the MPU chip.
		// mpuRate is the update rate in Hz.
		// magMix controls the amoutn of influence that the magnetometer has on yaw:
		//   0 = just use MPU gyros (will not be referenced to north)
		//   1 = just use magnetometer with no input from gyros
		//   2-n = mix the two. Higher numbers decrease correction from magnetometer
		// It returns false if something went wrong with the initialization.
		// magRate is the magnetometer update rate in Hz. magRate <= mpuRate.
		//   Also, magRate must be <= 100Hz.
		// lpf is the low pass filter setting - can be between 5Hz and 188Hz.
		//   0 means let the MotionDriver library decide.

		bool init( int mpuRate, int magMix = 5, int magRate = 10, int lpf = 0 );
		bool read();
		void disableAccelCal();

		bool isMagCal();
		bool isAccelCal();

		// Statistics
		void AddResult( EResult resultTypeIn );
        uint32_t GetResultCount( EResult resultTypeIn );
        void ClearResultCount( EResult resultTypeIn );

	private:
		// Result statistics
		orutil::TResultCount<EResult::_RESULT_COUNT, EResult::RESULT_SUCCESS> m_results;

		CALLIB_DATA 	m_calData;					// calibration data
		bool 			m_useMagCalibration;		// true if use mag calibration
		bool 			m_useAccelCalibration;		// true if use mag calibration
		uint8_t 		m_device;					// IMU device index
		int 			m_magMix;					// controls gyro and magnetometer mixing for yaw
		unsigned long 	m_magInterval;				// interval between mag reads in mS
		unsigned long 	m_lastMagSample;			// last time mag was read

		float 			m_lastDMPYaw;				// the last yaw from the DMP gyros
		float 			m_lastYaw;					// last calculated output yaw

		// Calibration data in processed form
		short 			m_magXOffset;				// offset to be structed for mag X
		short 			m_magXRange;				// range of mag X
		short 			m_magYOffset;				// offset to be structed for mag Y
		short 			m_magYRange;				// range of mag Y
		short 			m_magZOffset;				// offset to be structed for mag Z
		short 			m_magZRange;				// range of mag Z

		short 			m_accelXRange;				// range of accel X
		short 			m_accelYRange;				// range of accel Y
		short 			m_accelZRange;				// range of accel Z
		long 			m_accelOffset[3];			// offsets for accel

		// Fuse mag data with the dmp quaternion
		void dataFusion();                                        

	};
}

#endif // _MPU9150_H_
