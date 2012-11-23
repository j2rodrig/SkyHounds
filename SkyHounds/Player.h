/**
Player avatar.
*/

#ifndef PLAYER_H
#define PLAYER_H

#include "Avatar.h"

class Player : public Avatar {
public:
	Player (Script * _script, Scenario * _scenario);

	// What is the player aiming at?
	void SetAimTarget (float map_x, float map_y);

	// Where is the player moving to?
	void SetMotionTarget (float map_x, float map_y);

	// Fire the given weapon, if possible. (if weapon_id < 0, then do not fire.)
	// (More accurately: Registers an intention to fire the given weapon.)
	// Precondition: Player must possess weapon, and weapon must be ready to fire.
	void Fire (int weapon_id);
};

#endif
