#include "CPin.h"

CPin::CPin( uint8_t pinNumberIn, EPinType pinTypeIn, EPinDirection pinDirectionIn )
    : m_pinNumber( pinNumberIn )
    , m_pinType( pinTypeIn )
    , pinDirection( pinDirectionIn )
{
    static_assert( pinTypeIn < EPinType::PINTYPE_COUNT );
    static_assert( pinDirIn < EPinDirection::PINDIR_COUNT );

    // Set up digital pin directions
    if( m_pinType == EPinType::DIGITAL )
    {
        if( m_pinDirection == EPinDirection::INPUT )
        {
            pinMode( m_pinNumber, INPUT );
        }
        else
        {
            pinMode( m_pinNumber, OUTPUT );
        }
    }
}

uint32_t CPin::Read()
{
    if( m_pinDirection == EPinDirection::INPUT )
    {
        switch( m_pinType )
        {
            case EPinType::DIGITAL:
            {
                return digitalRead( m_pinNumber );
            }

            case EPinType::ANALOG:
            {
                return analogRead( m_pinNumber );
            }
            
            default:
            {
                return 0;
            }
        }
    }
    else
    {
        return 0;
    }
}

void CPin::Write( int valueIn )
{
    if( m_pinDirection == EPinDirection::OUTPUT )
    {
        switch( m_pinType )
        {
            case EPinType::DIGITAL:
            {
                if( valueIn == 0 )
                {
                    digitalWrite( m_pinNumber, LOW );
                }
                else
                {
                    digitalWrite( m_pinNumber, HIGH );
                }

                return;
            }

            case EPinType::ANALOG:
            {
                analogWrite( m_pinNumber, valueIn );
                return;
            }
            
            default:
            {
                return;
            }
        }
    }
}

void CPin::SetDirection( EPinDirection dirIn )
{
    static_assert( pinDirIn < EPinDirection::PINDIR_COUNT );

    m_pinDirection = dirIn;
}