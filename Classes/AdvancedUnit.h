#pragma once

#include "Unit.h"

class Fighter : public Unit
{
public:
	static Fighter* create(const std::string& filename);
	/**
	 * \brief "take off" before leave now point
	 */
	void motivate() override;
private:
	/**
	 * \brief set properties of fighter
	 */
	void setProperties() override;
	/**
	 * \brief fighter doesn't take up grid map
	 */
	void move() override;
	/**
	 * \brief fighter directly fly to somewhere
	 */
	GridPath findPath(const GridPoint& dest) const override;
};

class Tank : public Unit
{
public:
	static Tank* create(const std::string& filename);
private:
	GridSize splash_range;
	/**
	 * \brief tank make splash attack
	 */
	void attack() override;
	/**
	 * \brief set properties of tank
	 */
	void setProperties() override;
};

class Soldier : public Unit
{
public:
	static Soldier* create(const std::string& filename);
private:
	/**
	 * \brief set properties of soldier
	 */
	void setProperties() override;
};