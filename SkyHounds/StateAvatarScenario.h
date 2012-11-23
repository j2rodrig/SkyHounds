#ifndef STATE_AVATAR_SCENARIO_H
#define STATE_AVATAR_SCENARIO_H

// PScenarioAvatar: state updated by Scenario, readable by Avatar
struct StateScenarioAvatar {
	StateScenarioAvatar () { memset (this, 0, sizeof(StateScenarioAvatar)); }
	int team_assignment;                 // Which team am I on? (0=none)
	bool on_map;                         // Is this avatar spawned on the map?
	float map_x, map_y;                  // Where I am (on scenario map).
	float aim_x, aim_y;                  // What I am aiming at.
	// Other info... e.g., target in view?
};

// StateAvatarScenario: state updated by Avatar, readable by Scenario
struct StateAvatarScenario {
	StateAvatarScenario () { memset (this, 0, sizeof(StateAvatarScenario)); }
	int join_team;                       // Which team am I wanting to join? (0=none)
	bool playing;                        // Have I started playing yet?
	float motion_goal_x, motion_goal_y;  // Where do I want to go?
	float target_x, target_y;            // Where do I want to aim?
	int desired_weapon_id;               // What weapon do I want to use?
	bool fire_impulse;                   // Do I want to fire my weapon?
};

// CONTROL PROTOCOL:
// Avatar: join_team
// Scenario: (confirm) team_assignment
// Scenario: (spawn avatar) on_map
// Avatar: playing

// NETWORK NOTES:
// Only send an object if data has changed,
// and only if a certain amount of time has elapsed since last packet.

#endif
