#include "SysConfig.h"
#if(HAS_PCA9539)

#include <CI2C.h>
#include "CPCA9539.h"
#include "CTimer.h"
#include "NDataManager.h"

class BoolTimer : public CTimer
{
    uint32_t GetElapsed() const { return ( Now() - m_startTimeMs ); };
}


namespace
{
    CTimer pcaSampleTimer;
    BoolTimer bitTimer;
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
    bitTimer.Reset();
    
    //Expander init
    m_pca.Initialize();
    m_pca.PinMode( OUTPUT );

    Serial.println( "CPCA9539.Status:POST_INIT;");
}

void CPCA9539::Update( CCommand &commandIn )
{
    if( pcaSampleTimer.HasElapsed( 1000 ) )
    {
        auto elapsed = BoolTimer.GetElapsed();
        Serial.println( elapsed );
    }
}

#endif