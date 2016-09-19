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
    CTimer pca9539_sos_timer;


    //bool toggle = true;

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
void CPCA9539::character( int pin, int speed )
{
    m_pca.DigitalWrite(pin, HIGH);
    if( pca9539_sos_timer.HasElapsed( speed) )
    {
        m_pca.DigitalWrite(pin, LOW);
    }
}

void CPCA9539::Update( CCommand &commandIn )
{
    if( pca9539_sample_timer.HasElapsed( 2000 ) )
    {
        SOS();
    }
    
}

void CPCA9539::SOS()
{
    
    for( size_t i = 0; i < 5; ++i )
    {

        character(i, 300);
    }

}


void CPCA9539::KnightRider()
{
    //Delay in ms between flashes
    // auto delay = 20;
}


#endif