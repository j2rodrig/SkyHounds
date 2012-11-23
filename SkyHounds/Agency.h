/**
An Agency performs decision-making functions.
AI agencies make decisions based on inputs from an avatar.
Player agencies are controlled by players, either from console or from network
*/

#ifndef AGENCY_H
#define AGENCY_H

class Agency {
public:
	// SimTick is called once per frame
	// inputs: map and enemy surroundings. influence map properties at current position.
	// outputs: motions: north,south,east,west, aim, weapon select, fire.
	// actions/behaviour:
	//  - strength of impulse to perform certain task.
	//  - tendency to keep performing task: multiply winning task by exponential rise and decay. Lateral inhibition?
	void SimTick () { }
};

#endif