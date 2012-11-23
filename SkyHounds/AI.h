/**
Avatar with artificial intelligence.
*/

#ifndef AI_H
#define AI_H

#include "Avatar.h"

class AI : public Avatar {
public:
	AI (Script * _script, Scenario * _scenario);

	virtual void SimTick ();

};

#endif
