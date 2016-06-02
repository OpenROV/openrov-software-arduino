#pragma once

// Includes
#include <Arduino.h>
#include "SystemConstants.h"

class CPin
{
private:
    static const char kSeparator = ':';
    static const char kDelimeter = ';';

    String	m_name;

    int		m_pinNumber;
    int		m_value;

    boolean m_isDigital;
    boolean m_isInput;

public:
    static const boolean kAnalog	= false;
    static const boolean kDigital	= true;
    static const boolean kOutput	= false;
    static const boolean kInput		= true;

    CPin( String pin_name, int pin_number, boolean digital_truth, boolean in_out );
    CPin( String pin_name, int pin_number, boolean digital_truth );

    int Read();
    void Write( int val );

    void Send( int val );
    void Reset();

    String ToString();

};