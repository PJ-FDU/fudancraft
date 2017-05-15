#pragma once

#include "Unit.h"

USING_NS_CC;

class Fighter : public Unit
{
public:
	static Fighter* create(const std::string& filename);
private:
	void setProperties() override;
};