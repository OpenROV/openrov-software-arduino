#pragma once

// Includes
#include <Arduino.h>
#include "PinDefinitions.h"

class CPin
{
private:
    static const char kSeparator = ':';
    static const char kDelimeter = ';';

    int		m_pinNumber;
    int		m_value;

    bool m_isDigital;
    bool m_isInput;

public:
    static const bool kAnalog	= false;
    static const bool kDigital	= true;
    static const bool kOutput	= false;
    static const bool kInput		= true;

    CPin( int pin_number, bool digital_truth, bool in_out );
    CPin( int pin_number, bool digital_truth );

    int Read();
    void Write( int val );

    void Reset();
};