/**
A Creep is a non-player agent.
*/

#ifndef CREEP_H
#define CREEP_H

#include "Avatar.h"

class Creep : public Avatar {
public:
	Creep (Script * _script, Scenario * _scenario)
		: Avatar(_script, _scenario)
		{ }
};

#endif

