#include "libraries.h"

#include "Avatar.h"

#include "AI.h"
#include "Creep.h"
#include "Hero.h"
#include "Scenario.h"
#include "Script.h"

Avatar::Avatar (Script * _script, Scenario * _scenario)
	: MemoryBinding(_scenario), script(_script), scenario(_scenario)
{
}

// Advance simuation by a time-step.
void Avatar::SimTick (const PScenarioAvatar & p, VAvatarScenario & v) {
	avatar_agency->SimTick (p, v);
}

Avatar * Avatar::Make (std::string avatar_type, std::string agency_type, Scenario * scenario) {
	Script * script = new Script (std::string("avatars/") + avatar_type + "/script.txt");

	Avatar * avatar;
	if (avatar_type == "hero")
		avatar = new Hero (script, scenario);
	else if (avatar_type == "creep")
		avatar = new Creep (script, scenario);
	else {
		warning (NULL, "Unrecognized avatar_type: %s\n", avatar_type.c_str());
		breakpoint ();
		avatar = NULL;
	}

	AvatarAgency * agency;
	if (agency_type == "player")
		agency = new AvatarAgency (avatar);
	else if (agency_type == "ai")
		agency = new AI (avatar);
	else {
		warning (NULL, "Unrecognized agency_type: %s\n", agency_type.c_str());
		breakpoint ();
		agency = NULL;
	}

	avatar->set_agency (agency);

	return avatar;
}
