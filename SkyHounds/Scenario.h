/**
A Scenario holds all game instance information, including background map
and avatar information.
*/

#ifndef SCENARIO_H
#define SCENARIO_H

#include "Avatar.h"
#include "MemoryPool.h"
#include "StateAvatarScenario.h"

class Scenario : public MemoryPool {
	ALLEGRO_BITMAP * background;
	ALLEGRO_BITMAP * paths_image;
	int area_width, area_height;

	struct PerAvatar {
		Avatar * avatar;
		StateScenarioAvatar p;
		StateAvatarScenario v;
		PerAvatar (Avatar *_avatar) : avatar(_avatar) { }
	};
	std::vector<PerAvatar> perAvatar;
	std::map<Avatar*,int> perAvatarIndex;

public:
	Scenario (std::string name, std::string dropbox);

	int get_map_width () const { return area_width; }
	int get_map_height () const { return area_height; }

	virtual void onBinding (MemoryBinding * thing) {
		Avatar * avatar = dynamic_cast<Avatar*>(thing);
		if (avatar) {
			perAvatar.push_back (PerAvatar (avatar));
			perAvatarIndex[avatar] = (signed) perAvatar.size() - 1;
		}
	}

	virtual void onDeleting (MemoryBinding * thing) {
		Avatar * avatar = dynamic_cast<Avatar*>(thing);
		if (avatar) {
			perAvatar.erase (perAvatar.begin() + perAvatarIndex[avatar]);
			perAvatarIndex.erase (avatar);
		}
	}

	// Receives information from an avatar.
	void UpdateAvatarInfo (const StateAvatarScenario & v, Avatar * avatar);

	// Returns avatars that have requested to join the given team.
	std::vector<Avatar*> GetJoinList (int team_no);

	// Advances the scenario simulation by one time-step.
	virtual void SimTick ();

	// Displays map and objects.
	// target should already be selected on function entry.
	void Display (ALLEGRO_BITMAP * target);

private:
	// Loads a play area image.
	// All play area images should have the same dimensions.
	ALLEGRO_BITMAP * LoadAreaImage (std::string file_name);
};

#endif
