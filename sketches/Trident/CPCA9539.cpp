#include "SysConfig.h"
#if(HAS_PCA9539)

#include <CI2C.h>
#include "CPCA9539.h"
#include "CTimer.h"
#include "NDataManager.h"

namespace
{
    CTimer pca9539_sample_timer;
    

    //Morse code sos stuff
    uint8_t sosCounter = 0;
    bool isS = true; //s = true, o = false
    CTimer pca9539_s_timer;

    bool toggle = true;

}

CPCA9539::CPCA9539( CI2C *i2cInterfaceIn )
    : m_pca( i2cInterfaceIn )
{

}

void CPCA9539::Initialize()
{
    Serial.println( "CPCA9539.Status:INIT;" );

    //Timer resets
    pca9539_sample_timer.Reset();
    
    //Expander init
    m_pca.Initialize();
    m_pca.PinMode( OUTPUT );

    Serial.println( "CPCA9539.Status:POST_INIT;");
}

void CPCA9539::Update( CCommand &commandIn )
{
    SOS();
}

void CPCA9539::SOS()
{
    if( isS )
    {
        if( pca9539_s_timer.HasElapsed( 300 ) )
        {
            //Write them low
            for( size_t i = 0; i < 5; ++i )
            {
                m_pca.DigitalWrite( i, LOW );
            }
        }
        else
        {
            for( size_t i = 0; i < 5; ++i )
            {
                m_pca.DigitalWrite( i, HIGH );
            }
            ++sosCounter;
        }
        if( sosCounter = 3 )
        {
            isS = false
            sosCounter = 0;
        }
    }
    else
    {
        if( pca9539_s_timer.HasElapsed( 800 ) )
        {
            //Write them low
            for( size_t i = 0; i < 5; ++i )
            {
                m_pca.DigitalWrite( i, LOW );
            }
        }
        else
        {
            for( size_t i = 0; i < 5; ++i )
            {
                m_pca.DigitalWrite( i, HIGH );
            }
            ++sosCounter;
        }
        if( sosCounter = 3 )
        {
            isS = true
            sosCounter = 0;
        }
    }

}


void CPCA9539::KnightRider()
{
    //Delay in ms between flashes
    auto delay = 20;



}


#endif