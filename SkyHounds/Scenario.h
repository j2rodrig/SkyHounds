#ifndef SCENARIO_H
#define SCENARIO_H

#include "Avatar.h"
#include "MemoryPool.h"

// PScenarioAvatar: state updated by Scenario, readable by Avatar
struct PScenarioAvatar {
	float map_x, map_y;
	float aim_x, aim_y;
};

struct VAvatarScenario {
	float impulse_x, impulse_y;
	float target_x, target_y;
};

class Scenario : public MemoryPool {
	ALLEGRO_BITMAP * background;
	ALLEGRO_BITMAP * paths_image;
	int area_width, area_height;

	struct PerAvatar {
		Avatar * avatar;
		bool in_game;       // true implies p is initialized
		PScenarioAvatar p;
		VAvatarScenario v;
		PerAvatar (Avatar *_avatar) : avatar(_avatar), in_game (false) { }
	};
	std::vector<PerAvatar> perAvatar;
	std::map<Avatar*,int> perAvatarIndex;

public:
	Scenario (std::string name, std::string dropbox) : area_width(0), area_height(0) {
		// Locations of data.
		std::string folder = std::string("scenarios/") + name + "/";
		std::string image_folder = dropbox + "scenarios/" + name + "/";

		// Load scenario data.
		background = LoadAreaImage (image_folder + "background.png");
		//paths_image = LoadAreaImage (image_folder + "paths.png");

		// Perform processing. (collision map, etc?)
	}

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


	// Advances the scenario simulation by one time-step.
	void SimTick () {
		// Make each avatar think.
		int i;
		for (i = 0; i < (signed) perAvatar.size(); i++) {
			perAvatar[i].avatar->SimTick (perAvatar[i].p, perAvatar[i].v);
		}
		// Update simulation.
		for (i = 0; i < (signed) perAvatar.size(); i++) {
			// TO DO: collision resolution
		}
	}

	// Displays map and objects.
	// target should already be selected on function entry.
	void Display (ALLEGRO_BITMAP * target) {
		al_draw_bitmap (background, 0, 0, 0);
	}

private:
	// Loads a play area image.
	// All play area images should have the same dimensions.
	ALLEGRO_BITMAP * LoadAreaImage (std::string file_name) {
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
};

#endif
