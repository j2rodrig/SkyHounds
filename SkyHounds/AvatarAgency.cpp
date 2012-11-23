#include "libraries.h"

#include "AvatarAgency.h"

#include "Avatar.h"

AvatarAgency::AvatarAgency (Avatar * _avatar)
	: avatar(_avatar)
{
}

// Advance simuation by a time-step.
void AvatarAgency::SimTick (const PScenarioAvatar & p, VAvatarScenario & v) {
}

// What is the player aiming at?
void AvatarAgency::SetAimTarget (float map_x, float map_y, int issue_time) {
}

// Where is the player moving to?
void AvatarAgency::SetMotionTarget (float map_x, float map_y, int issue_time) {
}

// Fire a weapon.
// Precondition: Player must possess weapon, and weapon must be ready to fire.
void AvatarAgency::Fire (int weapon_id, int issue_time) {
}
