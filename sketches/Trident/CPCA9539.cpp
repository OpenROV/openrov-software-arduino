#include "SysConfig.h"
#if(HAS_PCA9539)

#include <CI2C.h>
#include "CPCA9539.h"
#include "CTimer.h"
#include "NDataManager.h"

namespace
{
    CTimer pcaSampleTimer;

    uint8_t counter;
    uint8_t value;

    bool firstPass;
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
    counter = 1;

    //Expander init
    m_pca.Initialize();
    m_pca.PinMode( OUTPUT );

    Serial.println( "CPCA9539.Status:POST_INIT;");
}

void CPCA9539::Update( CCommand &commandIn )
{
    if( pcaSampleTimer.HasElapsed( 500 ) )
    {
        KnightRider();       
    }
}

void CPCA9539::KnightRider()
{
    if( firstPass )
    {
        value = ~(counter << 1) & bitmask;

        auto ret = m_pca.DigitalWriteDecimal( value );
        if( ret != pca9539::ERetCode::SUCCESS )
        {
            Serial.println(ret);
        }

        if( counter == 16 )
        {
            firstPass = false;
        }
    } 
    else
    {
        value = ~(counter >> 1) & bitmask;

        auto ret = m_pca.DigitalWriteDecimal( counter );
        if( ret != pca9539::ERetCode::SUCCESS )
        {
            Serial.println(ret);
        }

        if( counter == 1 )
        {
            firstPass = true;
        }
    } 

}


#endif