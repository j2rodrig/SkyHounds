#include "libraries.h"

#include "Scenario.h"

Scenario::Scenario (std::string name, std::string dropbox) : area_width(0), area_height(0) {
	// Locations of data.
	std::string folder = std::string("scenarios/") + name + "/";
	std::string image_folder = dropbox + "scenarios/" + name + "/";

	// Load scenario data.
	background = LoadAreaImage (image_folder + "background.png");
	//paths_image = LoadAreaImage (image_folder + "paths.png");

	// Perform processing. (collision map, etc?)
}


// Receives information from an avatar.
void Scenario::UpdateAvatarInfo (const StateAvatarScenario & v, Avatar * avatar) {
	if (perAvatarIndex.count (avatar) == 0) {
		warning (this, "Avatar pointer not found in index");
		breakpoint ();
		return;
	}
	int i = perAvatarIndex[avatar];
	PerAvatar & per_avatar = perAvatar[i];
	per_avatar.v = v;
}

// Returns avatars that have requested to join the given team.
std::vector<Avatar*> Scenario::GetJoinList (int team_no) {
	std::vector<Avatar*> join_list;
	int i;
	for (i = 0; i < (signed) perAvatar.size(); i++) {
		if (perAvatar[i].v.join_team == team_no)
			join_list.push_back (perAvatar[i].avatar);
	}
	return join_list;
}

// Advances the scenario simulation by one time-step.
void Scenario::SimTick () {
	// Tell each avatar what its status in the scenario actually is.
	int i;
	for (i = 0; i < (signed) perAvatar.size(); i++) {
		perAvatar[i].avatar->UpdateScenarioInfo (perAvatar[i].p, this);
	}
	// Advance simulation time-step of avatars.
	for (i = 0; i < (signed) perAvatar.size(); i++) {
		perAvatar[i].avatar->SimTick ();
	}
	// Resolve avatar movements.
	for (i = 0; i < (signed) perAvatar.size(); i++) {
		// Only move avatar if actually playing.
		if (perAvatar[i].p.on_map && perAvatar[i].v.playing) {
			// TO DO: convert motion goal/target/desired weapon into actual movement
		}
	}
}

// Displays map and objects.
// target should already be selected on function entry.
void Scenario::Display (ALLEGRO_BITMAP * target) {
	al_draw_bitmap (background, 0, 0, 0);
	int i;
	for (i = 0; i < (signed) perAvatar.size(); i++) {
		// Only display avatar if actually on map.
		if (perAvatar[i].p.on_map) {
			// TO DO: display avatar
		}
	}
}

// Loads a play area image.
// All play area images should have the same dimensions.
ALLEGRO_BITMAP * Scenario::LoadAreaImage (std::string file_name) {
	ALLEGRO_BITMAP * bitmap = al_load_bitmap (file_name.c_str());
	if (! bitmap) {
		warning (this, "Can't load %s", file_name.c_str());
		breakpoint ();
	}
	if (area_width <= 0) {
		// Set area width and height
		area_width = al_get_bitmap_width (bitmap);
		area_height = al_get_bitmap_height (bitmap);
	} else {
		// Compare width and height with previously-loaded bitmap.
		if (al_get_bitmap_width (bitmap) != area_width ||
				al_get_bitmap_height (bitmap) != area_height) {
			warning (this, "Image %s dimensions (%d, %d) don't match initial (%d, %d)",
				file_name.c_str(), al_get_bitmap_width (bitmap),
				al_get_bitmap_height (bitmap), area_width, area_height);
			breakpoint ();
		}
	}
	return bitmap;
}
