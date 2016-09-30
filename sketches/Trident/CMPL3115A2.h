#pragma once

//Includes
#include <MPL3115A2.h>
#include "CModule.h"

class CI2C;

class CMPL3115A2 : public CModule
{
    public: 
        CMPL3115A2( CI2C *i2cInterfaceIn );
        virtual void Initialize();
        virtual void Update( CCommand &commandIn );
    
    private:
        mpl3115a2::MPL3115A2 m_mpl;

        void InitializeSensor();
};