#include "SysConfig.h"
#if(HAS_PCA9539)

#include <CI2C.h>
#include "CPCA9539.h"
#include "CTimer.h"
#include "NDataManager.h"

namespace
{
    CTimer pcaSampleTimer;

    namespace knight_rider
    {    
        //Knight rider stuff
        
        //Bit counter
        uint8_t counter;
        
        //Value that stores the mutated counter
        uint8_t value;

        //Bit mask for the PCA
        uint8_t bitmask = 0x1F;

        bool firstPass;
    }
}

CPCA9539::CPCA9539( CI2C *i2cInterfaceIn )
    : m_pca( i2cInterfaceIn )
{

}

void CPCA9539::Initialize()
{
    Serial.println( "CPCA9539.Status:INIT;" );

    //Timer resets
    pcaSampleTimer.Reset();

    //Knight rider stuff
    knight_rider::counter = 0;
    knight_rider::firstPass = true;

    //Expander init
    m_pca.Initialize();
    m_pca.PinMode( OUTPUT );

    Serial.println( "CPCA9539.Status:POST_INIT;");
}

void CPCA9539::Update( CCommand &commandIn )
{
    if( pcaSampleTimer.HasElapsed( 100 ) )
    {
        KnightRider();       
    }
}

void CPCA9539::KnightRider()
{
    if( knight_rider::firstPass )
    {
        if( knight_rider::counter == 0 )
        {
            knight_rider::counter = 2;
        }

        //For the PCA, 1 is low and 0 is high
        //So we need to not and then mask the counter to get the desired pattern
        knight_rider::value = ~(knight_rider::counter);
        knight_rider::value &= knight_rider::bitmask;

        //Write the desired pattern
        auto ret = m_pca.DigitalWriteDecimal( knight_rider::value );
        if( ret != pca9539::ERetCode::SUCCESS )
        {
            Serial.println(ret);
        }

        //Increase the counter
        knight_rider::counter <<= 1;

        if( knight_rider::counter == 16 )
        {
            //Start the second half of the pattern
            knight_rider::firstPass = false;
        }
    } 
    else
    {
        //For the PCA, 1 is low and 0 is high
        //So we need to not and then mask the counter to get the desired pattern
        knight_rider::value = ~(knight_rider::counter);
        knight_rider::value &= knight_rider::bitmask;

        //Write the desired pattern
        auto ret = m_pca.DigitalWriteDecimal( knight_rider::value );
        if( ret != pca9539::ERetCode::SUCCESS )
        {
            Serial.println(ret);
        }

        //Increase the counter
        knight_rider::counter >>= 1;

        if( knight_rider::counter == 0 )
        {
            //Reset the counter to the first section of the pattern
            knight_rider::firstPass = true;
        }
    } 
}


#endif