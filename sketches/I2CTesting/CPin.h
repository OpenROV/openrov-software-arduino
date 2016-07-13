#pragma once

// Includes
#include <Arduino.h>

#define MAX_PIN_OUTPUT_STRING_LENGTH			50

class CPin
{
private:
    static const char kSeparator = ':';
    static const char kDelimeter = ';';

    String	m_name;

    int		m_pinNumber;
    int		m_value;

    bool m_isDigital;
    bool m_isInput;

public:
    static const bool kAnalog	= false;
    static const bool kDigital	= true;
    static const bool kOutput	= false;
    static const bool kInput		= true;

    CPin( String pin_name, int pin_number, bool digital_truth, bool in_out );
    CPin( String pin_name, int pin_number, bool digital_truth );

    int Read();
    void Write( int val );

    void Send( int val );
    void Reset();

    String ToString();

};