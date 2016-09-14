#include "SysConfig.h"
#if(HAS_MPL3115A2)

#include <CI2C.h>
#include "CMPL3115A2.h"
#include "CTimer.h"
#include "NDataManager.h"

namespace
{
    CTimer mpl3115a2_report_timer;
    CTimer mpl3115a2_sample_timer;

    bool isInitialized = false;
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
    mpl3115a2_report_timer.Reset();

    Serial.println( "MPL3115A2.Status:POST_INIT; ");
}

void CMPL3115A2::InitializeSensor()
{
    //Attempt to init the sensor
    auto ret = m_mpl.Initialize();
    if( ret != mpl3115a2::ERetCode::SUCCESS )
    {
        Serial.println( "MPL3115A2_INIT_STATUS:FAILED;" );
    }
    else
    {
        Serial.println( "MPL3115A2_INIT_STATUS:SUCCESS;" );

        //Set the mode
        m_mpl.SetMode( mpl3115a2::EMode::BAROMETER );

        //Set the over sample rate_div
        m_mpl.SetOversampleRatio( mpl3115a2::EOversampleRatio::OSR_128 );

        //Enable event flags
        m_mpl.EnableEventFlags();
        
        isInitialized = true;
    }
}

void CMPL3115A2::Update( CCommand &commandIn )
{
    //1 Hz
    if( mpl3115a2_sample_timer.HasElapsed( 1000 ) )
    {
        if( !isInitialized )
        {
            if( mpl3115a2_report_timer.HasElapsed( 5000 ) )
            {
                Serial.println( "MPL3115A2.Status:TRYING;" );
                InitializeSensor();
            }
            return;
        }

        //Attempt to read the pressue and temp
        float pressure;
        float temp;
        auto ret = m_mpl.ReadPressureAndTemp(pressure, temp);
        if( ret != mpl3115a2::ERetCode::SUCCESS )
        {
            Serial.print( "MPL3115A2.Status:ERROR: " );
            Serial.print( ret );
            Serial.println( ";" );
            return;
        }
        
        Serial.print( "MPL3115A2.Value.pressure: " );
        Serial.print( pressure );
        Serial.println( " kPa;" );

        Serial.print( "MPL3115A2.Value.temp: " );
        Serial.print( temp );
        Serial.println( " C;" );
    }
}

#endif