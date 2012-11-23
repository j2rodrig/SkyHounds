/**
An Avatar interfaces with the game scenario.
The Avatar is deleted when the scenario is deleted.
*/

#ifndef AVATAR_H
#define AVATAR_H

#include "MemoryBinding.h"
#include "Script.h"
#include "StateAvatarScenario.h"

class Scenario;  // forward declaration

class Avatar : public MemoryBinding {
protected:
	Scenario * scenario;
	Script * script;

	StateAvatarScenario scenario_p;
	StateScenarioAvatar scenario_v;
	
	Avatar (Script * _script, Scenario * _scenario);

public:

	virtual ~Avatar () {
		delete script;
	}

	// Register an intention to join a given team.
	void JoinGame (int desired_team);

	// Receives information from the scenario.
	void UpdateScenarioInfo (const StateScenarioAvatar & v, Scenario * scenario);

	// Advance simuation by a time-step.
	virtual void SimTick ();

	// Makes an avatar of the given type within a scenario.
	static Avatar * Make (std::string avatar_type, std::string agency_type, Scenario * scenario);
};

#endif
