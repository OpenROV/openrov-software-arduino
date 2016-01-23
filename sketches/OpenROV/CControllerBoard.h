#pragma once

// Includes
#include "SysConfig.h"
#include "CModule.h"

// Load the correct pin definitions for this board
#if     CONTROLLERBOARD == CONTROLLERBOARD_CAPE
        #include "PinDefinitions_Cape.h"
        
#elif   CONTROLLERBOARD == CONTROLLERBOARD_25
        #include "PinDefinitions_25.h"
        
#elif   CONTROLLERBOARD == CONTROLLERBOARD_TRIDENT
        #include "PinDefinitions_Trident.h"
#endif


class CControllerBoard : public CModule
{
public:
    // Methods
    virtual void Initialize();
    virtual void Update( CCommand& commandIn );
};
