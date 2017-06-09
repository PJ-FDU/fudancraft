#pragma once

#include "Unit.h"

class Fighter : public Unit
{
public:
	static Fighter* create(const std::string& filename);
	void motivate() override;
private:
	void setProperties() override;
	void move() override;
	GridPath findPath(const GridPoint& dest) const override;
};

class Tank : public Unit
{
public:
	static Tank* create(const std::string& filename);
private:
	void setProperties() override;
};

class Soldier : public Unit
{
public:
	static Soldier* create(const std::string& filename);
private:
	void setProperties() override;
};