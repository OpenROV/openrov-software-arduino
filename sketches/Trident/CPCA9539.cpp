#include "SysConfig.h"
#if(HAS_PCA9539)

#include <CI2C.h>
#include "CPCA9539.h"
#include "CTimer.h"
#include "NDataManager.h"

namespace
{
    CTimer pcaSampleTimer;

    uint32_t start;
    bool toggle;
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
    start = millis();
    toggle = true;

    //Expander init
    m_pca.Initialize();
    m_pca.PinMode( OUTPUT );

    Serial.println( "CPCA9539.Status:POST_INIT;");
}

void CPCA9539::Update( CCommand &commandIn )
{
    if( pcaSampleTimer.HasElapsed( 1000 ) )
    {
        if( toggle )
        {
            toggle = false;
            auto ret = m_pca.DigitalWriteHex( 0x15 );
            Serial.println(ret);
        }
        else
        {
            toggle = true;
            auto ret = m_pca.DigitalWriteDecimal( 10 );
            Serial.println(ret);
        }
        
    }



}

#endif