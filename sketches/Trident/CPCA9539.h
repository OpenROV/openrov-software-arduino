#pragma once

//Includes
#include <PCA9539.h>
#include "CModule.h"

class CI2C;

class CPCA9539 : public CModule
{
    public: 
        CPCA9539( CI2C *i2cInterfaceIn );
        virtual void Initialize();
        virtual void Update( CCommand &commandIn );

        void KnightRider();
        void SOS();
    
    private:
        pca9539::PCA9539 m_pca;

        void InitializeSensor();
};