#include "CPin.h"

CPin::CPin( int pin_number, bool digital_truth, bool in_out )
{
    m_pinNumber = pin_number;
    m_isDigital = digital_truth;
    m_isInput = in_out;
}

CPin::CPin( int pin_number, bool digital_truth )
{
    m_pinNumber = pin_number;
    m_isDigital = digital_truth;
    m_isInput = kInput;  // default to input
}

void CPin::Reset()
{
    if( m_isDigital )
    {
        if( m_isInput )
        {
            pinMode( m_pinNumber, INPUT );
        }
        else
        {
            pinMode( m_pinNumber, OUTPUT );
        }
    }
}

int CPin::Read()
{
    if( m_isInput )
    {
        if( m_isDigital )
        {
            m_value = digitalRead( m_pinNumber );
        }
        else
        {
            m_value = analogRead( m_pinNumber );
        }

        return m_value;
    }

    return -1;
}

void CPin::Write( int val )
{
    if( !m_isInput )
    {
        if( m_isDigital )
        {
            if( val == 0 )
            {
                digitalWrite( m_pinNumber, LOW );
            }
            else
            {
                digitalWrite( m_pinNumber, HIGH );
            }
        }
        else
        {
            analogWrite( m_pinNumber, val );
        }
    }

    m_value = val;
}