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
            auto ret = m_pca.DigitalWrite( 0 , HIGH );
            Serial.println( ret );

            toggle = false;
        }
        else
        {
            auto ret = m_pca.DigitalWrite( 0 , LOW );
            Serial.println( ret );

            toggle = true;
        }
        
    }
}

#endif