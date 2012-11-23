#ifndef AI_H
#define AI_H

#include "AvatarAgency.h"

class AI : public AvatarAgency {
public:
	AI (Avatar * _avatar);

	void SimTick ();

};

#endif
