/**
An Avatar interfaces with the game scenario.
The Avatar is deleted when the scenario is deleted.
*/

#ifndef HERO_H
#define HERO_H

#include "Avatar.h"

class Hero : public Avatar {
public:
	Hero (Script * _script, Scenario * _scenario)
		: Avatar(_script, _scenario)
		{ }
};

#endif

