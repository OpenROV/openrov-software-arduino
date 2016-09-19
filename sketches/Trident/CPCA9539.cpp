#include "SysConfig.h"
#if(HAS_PCA9539)

#include <CI2C.h>
#include "CPCA9539.h"
#include "CTimer.h"
#include "NDataManager.h"

namespace
{
    CTimer pca9539_sample_timer;
    bool toggle = true;

}

CPCA9539::CPCA9539( CI2C *i2cInterfaceIn )
    : m_pca( i2cInterfaceIn )
{

}

void CPCA9539::Initialize()
{
    Serial.println( "CPCA9539.Status:INIT;" );

    pca9539_sample_timer.Reset();
    m_pca.Initialize();
    m_pca.PinMode( OUTPUT );

    Serial.println( "CPCA9539.Status:POST_INIT;");
}

void CPCA9539::Update( CCommand &commandIn )
{
    if( pca9539_sample_timer.HasElapsed( 1000 ) )
    {
        Serial.println( "PCA9539.Status:LOOP;" );
        if( toggle )
        {
            for( size_t i = 0; i < 8; ++i )
            {
                auto ret = m_pca.DigitalWrite( i , HIGH );
                Serial.println( ret );
            }

            toggle = false;
        }
        else
        {
            for( size_t i = 0; i < 8; ++i )
            {
                auto ret = m_pca.DigitalWrite( i , LOW );
                Serial.println( ret );
            }

            toggle = true;
        }
        
    }
}

void CPCA9539::KnightRider()
{
    //stub
}


#endif