#include "I2C_Driver.h"

// Status register
#define TWI_STATUS      (TWSR & 0xF8)

// Transaction start helpers
#define SLA_W(address)  (address << 1)
#define SLA_R(address)  ((address << 1) + 0x01)

// Bit helpers
#define cbi(sfr, bit)   (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit)   (_SFR_BYTE(sfr) |= _BV(bit))

// ------------------------
// AVR TWI Documentation
// See: http://www.avrbeginners.net/architecture/twi/twi.html
// ------------------------

// TWCR Register
// ------------------
// Bit 7 - TWINT: As described above; This is the TWI Interrupt Flag. It is set when the TWI finishes ANY bus operation and has to cleared (by writing a 1 to it) before a new operation can be started.
// Bit 6 - TWEA: TWI Enable Acknowledge; When the device receives data (as slave or as master), this bit has to be set if the next incoming byte should be ACKed and cleared for a NACK.
// Bit 5 - TWSTA: TWI Start; When a master has to generate a start condition, write this bit 1 together with TWEN and TWINT. The TWI hardware will generate a start condition and return the appropriate status code.
// Bit 4 - TWSTO: TWI Stop; Similar to TWSTA, but generates a Stop condition on the bus. TWINT is not set after generating a Stop condition.
// Bit 3 - TWWC: TWI Write Collision; Set by the TWI hardware when writing to the TWI Data Register TWDR while TWINT is high.
// Bit 2 - TWEN: Any bus operation only takes place when TWEN is written to 1 when accessing TWCR.
// Bit 1 - Reserved
// Bit 0 - TWIE: TWI Interrupt Enable; If this bit is set, the CPU will jump to the TWI reset vector when a TWI interrupt occurs.

using namespace i2c;

namespace i2c
{
    namespace avr
    {
        // Status Types
        enum ETWIStatus : uint8_t
        {
            TWI_START         		= 0x08,
            TWI_REPEATED_START		= 0x10,
            TWI_MT_SLA_ACK	    	= 0x18,
            TWI_MT_SLA_NACK	    	= 0x20,
            TWI_MT_DATA_ACK     	= 0x28,
            TWI_MT_DATA_NACK    	= 0x30,
            TWI_MR_SLA_ACK	    	= 0x40,
            TWI_MR_SLA_NACK	    	= 0x48,
            TWI_MR_DATA_ACK     	= 0x50,
            TWI_MR_DATA_NACK    	= 0x58,
            TWI_LOST_ARBITRATION    = 0x38
        };

        constexpr uint32_t kTimeout_ms = 25;

        EI2CResult Enable( EI2CBaudRate baudRateIn )
        {
            // Initialize TWI prescaler
            cbi( TWSR, TWPS0 );
            cbi( TWSR, TWPS1 );

            // Apply default baud rate
            if( SetBaudRate( baudRateIn ) )
            {
                Disable();
                return EI2CResult::RESULT_ERR_INVALID_BAUD;
            }

            // Enable TWI module and acks
            TWCR = _BV( TWEN ) | _BV( TWEA );

            //Serial.println( "i2c:enabled;" );

            return EI2CResult::RESULT_SUCCESS;
        }

        EI2CResult Disable()
        {
            // Disables the TWI interface and TWI interrupts
            TWCR = 0;

            //Serial.println( "i2c:disabled;" );

            return EI2CResult::RESULT_SUCCESS; 
        }

        EI2CResult SetBaudRate( EI2CBaudRate baudRateIn )
        {
            // Set baud rate
            if( baudRateIn == EI2CBaudRate::BAUDRATE_100K )
            {
                TWBR = ( ( F_CPU / 100000 ) - 16 ) / 2;
            } 
            else if( baudRateIn == EI2CBaudRate::BAUDRATE_400K )
            {
                TWBR = ( ( F_CPU / 400000 ) - 16 ) / 2;
            }
            else
            {
                return EI2CResult::RESULT_ERR_INVALID_BAUD;
            }

            //Serial.println( "i2c:setbaud;" );

	        return EI2CResult::RESULT_SUCCESS;
        }

        EI2CResult StartTransaction()
        {
            EI2CResult ret;
            uint8_t status = 0;

            //Serial.println( "i2c:start;" );

            // Send start
            TWCR = ( 1 << TWINT ) | ( 1 << TWSTA ) | ( 1 << TWEN );

            // Wait for interrupt
            ret = WaitForInterrupt( status );

            if( ret )
		    {
                // Timeout
                //Serial.println( "i2c:timeout;" );
                Reset();
                return ret;
            }
            else
            {
                // Successful transaction start
                if( ( status == ETWIStatus::TWI_START ) || ( status == ETWIStatus::TWI_REPEATED_START ) )
                {
                    //Serial.println( "i2c:ok;" );
                    return EI2CResult::RESULT_SUCCESS;
                }
                
                // Handle errors
                if( status == ETWIStatus::TWI_LOST_ARBITRATION )
                {
                    // Lost Arbitration
                    //Serial.println( "i2c:arb;" );
                    Reset();
                    return EI2CResult::RESULT_ERR_LOST_ARBITRATION;
                }
                else
                {
                    // Unknown error
                    //Serial.println( "i2c:fail;" );
                    Reset();
                    return EI2CResult::RESULT_ERR_FAILED;
                }
            }
        }

        EI2CResult SendAddress( uint8_t slaveAddressIn, EI2CAction actionIn )
        {
            EI2CResult ret;
            uint8_t status = 0;

            //Serial.println( "i2c:sendaddr;" );

            // Set slave address
            TWDR = ((slaveAddressIn << 1) + actionIn );
            TWCR = ( 1 << TWINT ) | ( 1 << TWEN );

            // Wait for interrupt
            ret = WaitForInterrupt( status );

            if( ret )
		    {
                // Timeout
                //Serial.println( "i2c:timeout;" );
                Reset();
                return ret;
            }
            else
            {
                // Success
                if( ( status == ETWIStatus::TWI_MT_SLA_ACK ) || ( status == ETWIStatus::TWI_MR_SLA_ACK ) )
                {
                    //Serial.println( "i2c:ok;" );
                    return EI2CResult::RESULT_SUCCESS;
                }

                // Handle errors
                if( ( status == ETWIStatus::TWI_MT_SLA_NACK ) || ( status == ETWIStatus::TWI_MR_SLA_NACK ) )
                {
                    // Nack received. End transaction
                    //Serial.println( "i2c:nack;" );
                    StopTransaction();
                    return EI2CResult::RESULT_NACK;
                }
                else
                {
                    // Unknown error
                    //Serial.println( "i2c:fail;" );
                    Reset();
                    return EI2CResult::RESULT_ERR_FAILED;
                }
            }
        }

        EI2CResult WriteByte( uint8_t byteIn )
        {
            EI2CResult ret;
            uint8_t status = 0;

            //Serial.println( "i2c:write;" );

            // Write byte to data register
            TWDR = byteIn;
	        TWCR = ( 1 << TWINT ) | ( 1 << TWEN );

            // Wait for interrupt
            ret = WaitForInterrupt( status );

            if( ret )
		    {
                // Timeout
                //Serial.println( "i2c:timeout;" );
                Reset();
                return ret;
            }
            else
            {
                // Success
                if( status == ETWIStatus::TWI_MT_DATA_ACK )
                {
                    //Serial.println( "i2c:ok;" );
                    return EI2CResult::RESULT_SUCCESS;
                }

                // Handle errors
                if( status == ETWIStatus::TWI_MT_DATA_NACK )
                {
                    //Serial.println( "i2c:nack;" );

                    // Nack received. End transaction
                    StopTransaction();
                    return EI2CResult::RESULT_NACK;
                }
                else
                {
                    // Unknown error
                    //Serial.println( "i2c:fail;" );
                    Reset();
                    return EI2CResult::RESULT_ERR_FAILED;
                }
            }
        }

        EI2CResult ReadByte( uint8_t* dataOut, bool sendNack )
        {
            EI2CResult ret;
            uint8_t status = 0;

            //Serial.println( "i2c:read;" );

            // Handle ACK/NACK
            if( sendNack )
            {
                // If this is the final read in a transfer, send a NACK
                TWCR = ( 1 << TWINT ) | ( 1 << TWEN );
            }
            else
            {
                // Otherwise, send an ACK
                TWCR = ( 1 << TWINT ) | ( 1 << TWEN ) | ( 1 << TWEA );
            }

            // Wait for interrupt
            ret = WaitForInterrupt( status );

            if( ret )
		    {
                // Timeout
                //Serial.println( "i2c:timeout;" );
                Reset();
                return ret;
            }
            else
            {
                if( sendNack )
                {
                    // Success
                    if( status == ETWIStatus::TWI_MR_DATA_NACK )
                    {
                        //Serial.println( "i2c:ok;" );

                        // Read data byte into buffer
                        *dataOut = TWDR;
                        return EI2CResult::RESULT_SUCCESS;
                    }
                }
                else
                {
                    // Success
                    if( status == ETWIStatus::TWI_MR_DATA_ACK )
                    {
                        //Serial.println( "i2c:ok;" );

                        // Read data byte into buffer
                        *dataOut = TWDR;
                        return EI2CResult::RESULT_SUCCESS;
                    }
                }

                if( status == ETWIStatus::TWI_LOST_ARBITRATION )
                {
                    // Lost arbitration
                    //Serial.println( "i2c:arb;" );
                    Reset();
                    return EI2CResult::RESULT_ERR_LOST_ARBITRATION;
                }
                else
                {
                    // Unknown error
                    //Serial.println( "i2c:fail;" );
                    Reset();
                    return EI2CResult::RESULT_ERR_FAILED;
                }
            }
        }

        EI2CResult StopTransaction()
        {
            EI2CResult ret;

            //Serial.println( "i2c:stop;" );

            // Send Stop
            TWCR = ( 1 << TWINT ) | ( 1 << TWEN ) | ( 1 << TWSTO );

            // Wait for stop to clear
            ret = WaitForStop();

            if( ret )
		    {
                // Timeout
                //Serial.println( "i2c:timeout;" );
                Reset();
                return ret;
            }

            //Serial.println( "i2c:ok;" );
            return EI2CResult::RESULT_SUCCESS;
        }

        EI2CResult WaitForStop()
        {
            uint32_t startTime = millis();

            //Serial.println( "i2c:wfs;" );

            // Wait for TWSTO to be cleared
            while( ( TWCR & ( 1 << TWSTO ) ) )
            {
                // Check for timeout
                if( ( millis() - startTime ) > kTimeout_ms )
                {
                    //Serial.println( "i2c:wfs_to;" );
                    return EI2CResult::RESULT_ERR_TIMEOUT;
                }
            }

            //Serial.println( "i2c:wfs_ok;" );

            return EI2CResult::RESULT_SUCCESS;
        }

        EI2CResult WaitForInterrupt( uint8_t &twiStatusOut )
        {
            uint32_t startTime = millis();

            //Serial.println( "i2c:wfi;" );

            // Wait for TWINT to be set
            while( !( TWCR & ( 1 << TWINT ) ) )
            {
                // Check for timeout
                if( ( millis() - startTime ) > kTimeout_ms )
                {
                    //Serial.println( "i2c:wfi_to;" );
                    return EI2CResult::RESULT_ERR_TIMEOUT;
                }
            }

            // Return the status
            twiStatusOut = TWI_STATUS;

            //Serial.println( "i2c:wfi_ok;" );

            return EI2CResult::RESULT_SUCCESS;
        }

        void Reset()
        {
            //Serial.println( "i2c:reset;" );
            // Disables TWI and releases SDA and SCL lines to high impedance
            TWCR = 0;

            // Reinitializes TWI
            TWCR = _BV( TWEN ) | _BV( TWEA ); 
        }
    }
}
