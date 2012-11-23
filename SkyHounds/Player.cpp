#include "libraries.h"

#include "Player.h"

Player::Player (Script * _script, Scenario * _scenario)
	: Avatar(_script,_scenario) {
}

// What is the player aiming at?
void Player::SetAimTarget (float map_x, float map_y) {
	scenario_p.target_x = map_x;
	scenario_p.target_y = map_y;
}

// Where is the player moving to?
void Player::SetMotionTarget (float map_x, float map_y) {
	scenario_p.motion_goal_x = map_x;
	scenario_p.motion_goal_y = map_y;
}

// Fire the given weapon, if possible. (if weapon_id < 0, then do not fire.)
// (More accurately: Registers an intention to fire the given weapon.)
// Precondition: Player must possess weapon, and weapon must be ready to fire.
void Player::Fire (int weapon_id) {
	if (weapon_id < 0)
		scenario_p.fire_impulse = false;
	else {
		scenario_p.desired_weapon_id = weapon_id;
		scenario_p.fire_impulse = true;
	}
}
