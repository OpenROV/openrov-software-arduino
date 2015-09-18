#pragma once

// Includes
#include "CModule.h"

class CMS5803_14BA : public CModule
{
public:
	void Initialize();
	void Update( CCommand& commandIn );
};
