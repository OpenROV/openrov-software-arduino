#pragma once

#include <Arduino.h>

class CModule
{
public:
	// Attributes
	String		m_name;
	uint32_t	m_executionTime;

	// Methods
	CModule();
	virtual ~CModule()
	{
	}

	// Pure virtuals - These methods must be instantiated in your derived class!
	virtual void Initialize() = 0;
	virtual void Update() = 0;
};
