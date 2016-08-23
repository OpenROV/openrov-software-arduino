
/**************************************************************************
 * Copyright (c) the authors of libfixmath as seen on https://code.google.com/p/libfixmath/
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 ***************************************************************************/

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

	#include <stdint.h>

	#define FIXMATH_NO_OVERFLOW
	#define FIXMATH_NO_ROUNDING

	typedef int32_t fix16_t;

	#define FOUR_DIV_PI  				0x145F3           	/*!< Fix16 value of 4/PI */
	#define _FOUR_DIV_PI2				0xFFFF9840       	/*!< Fix16 value of -4/PI² */
	#define X4_CORRECTION_COMPONENT		0x399A 				/*!< Fix16 value of 0.225 */
	#define PI_DIV_4					0x0000C90F          /*!< Fix16 value of PI/4 */
	#define THREE_PI_DIV_4				0x00025B2F      	/*!< Fix16 value of 3PI/4 */

	#define fix16_max					0x7FFFFFFF 			/*!< the maximum value of fix16_t */
	#define fix16_min					0x80000000 			/*!< the minimum value of fix16_t */
	#define fix16_overflow				0x80000000 			/*!< the value used to indicate overflows when FIXMATH_NO_OVERFLOW is not specified */

	#define fix16_pi					205887     			/*!< fix16_t value of pi */
	#define fix16_e						78145     			/*!< fix16_t value of e */
	#define fix16_one					0x00010000 			/*!< fix16_t value of 1 */
	#define fix16_neg_one				0xFFFF0000			/*!< fix16_t value of -1 */

	/* Conversion functions between fix16_t and float/integer.
	* These are inlined to allow compiler to optimize away constant numbers */
	static inline fix16_t fix16_from_int(int a) 	{ return a * fix16_one; }
	static inline float fix16_to_float(fix16_t a) 	{ return (float)a / fix16_one; }
	static inline double fix16_to_dbl(fix16_t a) 	{ return (double)a / fix16_one; }

	static inline int fix16_to_int(fix16_t a)
	{
		#ifdef FIXMATH_NO_ROUNDING
			return a >> 16;
		#else
			if (a >= 0)
				return (a + fix16_one / 2) / fix16_one;
			else
				return (a - fix16_one / 2) / fix16_one;
		#endif
	}

	static inline fix16_t fix16_from_float(float a)
	{
		float temp = a * fix16_one;
		
		#ifndef FIXMATH_NO_ROUNDING
			temp += (temp >= 0) ? 0.5f : -0.5f;
		#endif
		
		return (fix16_t)temp;
	}

	static inline fix16_t fix16_from_dbl(double a)
	{
		double temp = a * fix16_one;
		
		#ifndef FIXMATH_NO_ROUNDING
			temp += (temp >= 0) ? 0.5f : -0.5f;
		#endif
		
		return (fix16_t)temp;
	}

#ifdef __cplusplus
}
#endif
