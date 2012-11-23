#ifndef AVATAR_AGENCY_H
#define AVATAR_AGENCY_H

#include "Agency.h"

class Avatar;  // forward declaration
struct PScenarioAvatar;
struct VAvatarScenario;

class AvatarAgency : public Agency {
protected:
	Avatar * avatar;

	float aim_target_x;
	float aim_target_y;
	int aim_target_issue_time;
	bool aim_target_pending;

	float motion_target_x;
	float motion_target_y;
	int motion_target_issue_time;
	bool motion_target_pending;

	int fire_weapon_id;
	int fire_issue_time;
	bool fire_pending;

public:
	AvatarAgency (Avatar * _avatar);

	// Advance simuation by a time-step.
	void SimTick (const PScenarioAvatar & p, VAvatarScenario & v);

	// What is the player aiming at?
	void SetAimTarget (float map_x, float map_y, int issue_time);

	// Where is the player moving to?
	void SetMotionTarget (float map_x, float map_y, int issue_time);

	// Fire a weapon.
	// Precondition: Player must possess weapon, and weapon must be ready to fire.
	void Fire (int weapon_id, int issue_time);
};

#endif
