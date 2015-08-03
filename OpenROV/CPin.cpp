#include "CPin.h"

CPin::CPin( String module_name, int pin_number, boolean digital_truth, boolean in_out )
{
    m_name = module_name;
    m_pinNumber = pin_number;
    m_isDigital = digital_truth;
    m_isInput = in_out;
}

CPin::CPin( String module_name, int pin_number, boolean digital_truth )
{
    m_name = module_name;
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

void CPin::Send( int val )
{
    char output[MAX_PIN_OUTPUT_STRING_LENGTH];
    String temp = m_name + kSeparator + ( String )val + kDelimeter;
    temp.toCharArray( output, MAX_PIN_OUTPUT_STRING_LENGTH - 1 );

    Serial.write( output );
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

String CPin::ToString()
{
    String dig = ( m_isDigital ) ? "1" : "0";
    String inp = ( m_isInput ) ? "1" : "0";
    String temp = inp + "__" + dig + "__" + ( String )m_value;
    return temp;
}
