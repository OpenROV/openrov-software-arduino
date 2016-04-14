// Includes
#include "LibMS5837_30BA.h"
#include "CI2C.h"

// For I2C, set the CSB Pin (pin 3) high for address 0x76, and pull low for address 0x77.
#define I2C_ADDRESS             0x76    // or 0x77

#define CMD_RESET               0x1E
#define CMD_ADC_READ            0x00
#define CMD_ADC_CONV_BASE	    0x40	// ADC conversion base command. Modify based on D1/D2 and resolution

#define CMD_ADC_D1              0x00
#define CMD_ADC_D2              0x10

#define CMD_ADC_256             0x00	// ADC resolution = 256
#define CMD_ADC_512             0x02	// ADC resolution = 512
#define CMD_ADC_1024	        0x04	// ADC resolution = 1024
#define CMD_ADC_2048        	0x06	// ADC resolution = 2048
#define CMD_ADC_4096	        0x08	// ADC resolution = 4096
#define CMD_ADC_8192	        0x0A	// ADC resolution = 8192

// TODO: Add more error handling

int MS5837_30BA::Initialize()
{
    // First Reset
    Reset();
    
    
}

int MS5837_30BA::Reset()
{
    if( WriteRegisterByte( CMD_RESET ) )
    {
        return 1;
    }

    // Specified by data sheet
	delay( 10 );
	
	return 0;
}

int MS5837_30BA::GetCalibrationCoefficients()
{
    bool ret = true;
	bool gotCRC = true;

	// Read sensor coefficients
	for( int i = 0; i < 8; i++ )
	{
		// The PROM starts at address 0xA0
		if( i != 7 )
		{
			ret &= WriteRegisterByte( 0xA0 + ( i * 2 ) );
		}
		else
		{
			gotCRC &= WriteRegisterByte( 0xA0 + ( i * 2 ) );
		}

		if( I2c.read( MS5803_I2C_ADDRESS, 2 ) )
		{
			// Don't treat the crc read as an overall failure (yet)
			if( i != 7 )
			{
				ret &= false;
			}
			else
			{
				gotCRC &= false;
			}

			Serial.print( "Depth.CoeffFailure:C" );
			Serial.print( i );
			Serial.println( ";" );

			// Print out coefficients
			Serial.print( "Depth.C" );
			Serial.print( i );
			Serial.println( ":INVALID;" );
			delay( 10 );
		}
		else
		{
			// Get coefficient bytes
			while( I2c.available() )
			{
				HighByte = I2c.receive();
				LowByte = I2c.receive();
			}

			// Combine bytes
			sensorCoeffs[i] = ( ( ( unsigned int )HighByte << 8 ) + LowByte );

			// Print out coefficient
			Serial.print( "Depth.C" );
			Serial.print( i );
			Serial.print( ":" );
			Serial.print( sensorCoeffs[i] );
			Serial.println( ";" );
			delay( 10 );
		}
	}

	if( gotCRC == true )
	{
		// The last 4 bits of the 7th coefficient form a CRC error checking code.
		unsigned char p_crc = sensorCoeffs[7];
		// Use a function to calculate the CRC value
		unsigned char n_crc = MS_5803_CRC( sensorCoeffs );

		// If the CRC value doesn't match the sensor's CRC value, then the
		// connection can't be trusted. Check your wiring.
		if( p_crc != n_crc )
		{
			Serial.println( "Depth.crcCheck:Failed;" );
			//ret &= false;
		}
		else
		{
			Serial.println( "Depth.crcCheck:Succeeded;" );
		}
	}
	else
	{
		Serial.println( "Depth.crcCheck:CantPerform;" );
	}

	if( ret )
	{
		// Correct between 14mbar/30mbar sensor type
		if( sensorCoeffs[0] != 0 )
		{
			m_is30bar = true;
			m_divisor = 8192; // (2^13)
		}
		else
		{
			m_is30bar = false;
			m_divisor = 32768; // (2^15)
		}
	}

	// Otherwise, return true when everything checks out OK.
	return ret;
}

int MS5837_30BA::StartConversion()
{
    
}

int MS5837_30BA::Read()
{
    
}

void MS5837_30BA::SetWaterType( int waterTypeIn )
{
    
}

uint8_t MS5837_30BA::GetCRC( uint32_t n_prom[] )
{
    int cnt; // simple counter
    uint32_t n_rem=0; // crc remainder
    uint8_t n_bit;
    
    n_prom[0]=((n_prom[0]) & 0x0FFF); // CRC byte is replaced by 0
    n_prom[7]=0; // Subsidiary value, set to 0
    
    for (cnt =0; cnt < 16; cnt++) // operation is performed on bytes
    { 
        // choose LSB or MSB
        if (cnt%2==1) 
            n_rem ^= (uint16_t) ((n_prom[cnt>>1]) &0x00FF);
        else 
            n_rem ^= (uint16_t) (n_prom[cnt>>1]>>8);
            
        for (n_bit = 8; n_bit > 0; n_bit--)
        {
            if (n_rem & (0x8000)) 
                n_rem = (n_rem << 1) ^ 0x3000;
            else 
                n_rem = (n_rem << 1);
        }
    }
    
    n_rem= ((n_rem >> 12) & 0x000F); // final 4-bit remainder is CRC code
    
    return (n_rem ^ 0x00);
}

uint32_t MS5837_30BA::CommandADC( uint8_t commandIn )
{
    // Send the read command
	WriteRegisterByte( CMD_ADC_READ );

	// Then request the results. This should be a 24-bit result (3 bytes)
	I2c.read( I2C_ADDRESS, 3 );

	while( I2c.available() )
	{
		HighByte = I2c.receive();
		MidByte = I2c.receive();
		LowByte = I2c.receive();
	}

	// Combine the bytes into one integer
	result = ( ( uint32_t )HighByte << 16 ) + ( ( uint32_t )MidByte << 8 ) + ( uint32_t )LowByte;
	return result;
}

int MS5837_30BA::WriteRegisterByte( uint8_t addressIn )
{
    return I2c.write( ( uint8_t )I2C_ADDRESS, addressIn );
}

int MS5837_30BA::WriteDataByte( uint8_t addressIn, uint8_t dataIn )
{
    return I2c.write( ( uint8_t )I2C_ADDRESS, addressIn, dataIn );
}

int MS5837_30BA::ReadByte( uint8_t addressIn, uint8_t& dataOut )
{
    // Set address to request from
	uint8_t ret = I2c.read( ( uint8_t )MS5803_I2C_ADDRESS, ( uint8_t )addressIn, ( uint8_t )1 );

	// Non-zero failure
	if( ret )
	{
		return ret;
	}

	// Request single byte from slave
	dataOut = I2c.receive();

	return ret;
}