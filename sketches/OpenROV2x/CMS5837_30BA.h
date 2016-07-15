#pragma once

// Includes
#include "CModule.h"

class CMS5837_30BA : public CModule
{
public:
	void Initialize();
	void Update( CCommand& commandIn );
};
