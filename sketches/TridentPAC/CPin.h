#pragma once

// Includes
#include <Arduino.h>

// TODO: Eventually make GPIO handled with templates so the different pin types can be sorted out at compile time
class CPin
{
public:
    enum class EPinType
    {
        DIGITAL,
        ANALOG,
        PINTYPE_COUNT
    };

    enum class EPinDirection
    {
        INPUT,
        OUTPUT,
        PINDIR_COUNT
    };

    CPin( uint8_t pinNumberIn, EPinType pinTypeIn, EPinDirection pinDirectionIn = EPinDirection::INPUT );

    int32_t Read();
    void Write( uint32_t valueIn );

    void SetDirection( EPinDirection dirIn );

private:
    const uint8_t       m_pinNumber;
    const EPinType      m_pinType;
    
    EPinDirection       m_pinDirection;
};