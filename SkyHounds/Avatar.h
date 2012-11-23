/**
An Avatar interfaces with the game scenario.
The Avatar is deleted when the scenario is deleted.
*/

#ifndef AVATAR_H
#define AVATAR_H

#include "AvatarAgency.h"
#include "MemoryBinding.h"
#include "Script.h"

class Scenario;  // forward declaration
struct PScenarioAvatar;
struct VAvatarScenario;

// Variables accessed by Scenario and AvatarAgency
struct AvatarData {
	float map_x;
	float map_y;
	float impulse_x;
	float impulse_y;
};

class Avatar : public MemoryBinding {
protected:
	Scenario * scenario;
	Script * script;
	AvatarAgency * avatar_agency;
	
	Avatar (Script * _script, Scenario * _scenario);

	void set_agency (AvatarAgency * _avatar_agency) {
		avatar_agency = _avatar_agency;
	}

public:

	virtual ~Avatar () {
		delete avatar_agency;
		delete script;
	}

	// agency () returns direct controls for this avatar.
	AvatarAgency * agency () const { return avatar_agency; }

	// Advance simuation by a time-step.
	void SimTick (const PScenarioAvatar & p, VAvatarScenario & v);

	// Makes an avatar of the given type within a scenario.
	static Avatar * Make (std::string avatar_type, std::string agency_type, Scenario * scenario);
};

#endif
