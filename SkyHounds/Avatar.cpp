#include "libraries.h"

#include "Avatar.h"

#include "AI.h"
#include "Player.h"
#include "Scenario.h"
#include "Script.h"

Avatar::Avatar (Script * _script, Scenario * _scenario)
	: MemoryBinding(_scenario), script(_script), scenario(_scenario) {
}

// Register an intention to join a given team.
void Avatar::JoinGame (int desired_team) {
	scenario_p.join_team = desired_team;
	scenario->UpdateAvatarInfo (scenario_p, this);
}

// Receives information from the scenario.
void Avatar::UpdateScenarioInfo (const StateScenarioAvatar & v, Scenario * scenario) {
	scenario_v = v;
}

// Advance simuation by a time-step.
void Avatar::SimTick () {
	// Make sure the scenario is aware of any changes the avatar has made.
	scenario->UpdateAvatarInfo (scenario_p, this);
}

Avatar * Avatar::Make (std::string avatar_type, std::string agency_type, Scenario * scenario) {
	// Load the script for the avatar type.
	Script * script = new Script (std::string("avatars/") + avatar_type + "/script.txt");

	// Decide what kind of agency the avatar has: player or ai.
	// Avatar may ignore certain script commands, depending on agency type.
	Avatar * avatar;
	if (agency_type == "player")
		avatar = new Player (script, scenario);
	else if (agency_type == "ai")
		avatar = new AI (script, scenario);
	else {
		warning (NULL, "Unrecognized agency_type: %s\n", agency_type.c_str());
		breakpoint ();
		avatar = NULL;
	}

	return avatar;
}
