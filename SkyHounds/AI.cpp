#include "libraries.h"

#include "AI.h"

AI::AI (Script * _script, Scenario * _scenario)
	: Avatar(_script,_scenario) {
}

// Advance simuation by a time-step.
void AI::SimTick () {
	// If playing, decide what to do next.
	if (scenario_v.on_map) {
		// TO DO: Decide how to update scenario_p parameters.
	}
	// Do lower-level simulation.
	Avatar::SimTick ();
}
