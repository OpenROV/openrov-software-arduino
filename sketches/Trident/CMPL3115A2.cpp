#include "SysConfig.h"
#if(HAS_MPL3115A2)

#include <CI2C.h>
#include "CMPL3115A2.h"
#include "CTimer.h"
#include "NDataManager.h"

namespace
{
    CTimer mpl3115a2_sample_timer;
}



CMPL3115A2::CMPL3115A2( CI2C *i2cInterfaceIn )
    : m_mpl( i2cInterfaceIn )
{

}

void CMPL3115A2::Initialize()
{
    Serial.println( "MPL3115A2.Status:INIT;" );

    //init process
    mpl3115a2_sample_timer.Reset();

    Serial.println( "MPL3115A2.Status:POST_INIT; ");
}

void CMPL3115A2::Update( CCommand &commandIn )
{
    if( mpl3115a2_sample_timer.HasElapsed( 10 ) )
    {
        Serial.println( "MPL3115A2.Status:Update;" );
    }
    
}

#endif