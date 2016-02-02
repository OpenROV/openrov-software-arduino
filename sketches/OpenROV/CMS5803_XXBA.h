#pragma once

// Includes
#include "CModule.h"

class CMS5803_XXBA : public CModule
{
public:
	void Initialize();
	void Update( CCommand& commandIn );
};
