#pragma once

#include "Unit.h"

class Fighter : public Unit
{
public:
	static Fighter* create(const std::string& filename);
private:
	void setProperties() override;
};