#pragma once

// Includes
#include "CModule.h"

// Forward Declarations
class TwoWire;

class CBNO055 : public CModule
{
public:
	CBNO055( TwoWire *wireInterfaceIn );
	void Initialize();
	void Update();
};



