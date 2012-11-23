/**
 * Control class translates raw UI events into game events.
 */
#ifndef CONTROL_H
#define CONTROL_H

#include "Scenario.h"

class Control {
protected:
	Scenario * scenario;

	int current_time;

	int north;
	int south;
	int west;
	int east;

	float centre_x, centre_y;
	float zoom;

public:
	Control (Scenario * _scenario, ALLEGRO_DISPLAY * display)
		: current_time(0), north(0), south(0), west(0), east(0),
		  centre_x(al_get_display_width (display) / 2),
		  centre_y(al_get_display_height (display) / 2),
		  zoom(1.0f),
		  scenario(_scenario)
		{}

	void Mouse (int x, int y) {
	}

	void MouseScrollUp (int amount) {
		zoom += 0.1f * amount;
		if (zoom > 3.0f)
			zoom = 3.0f;
	}

	void MouseScrollDown (int amount) {
		zoom -= 0.1 * amount;
		if (zoom < 0.1f)
			zoom = 0.1f;
	}

	void MouseButton (int button) {
	}

	void LostMouse () {
	}

	void GotMouse () {
	}

	void Character (int c) {
	}

	void ControlCharacter (int c) {
	}

	void KeyDown (int k) {
		switch (k) {
		case ALLEGRO_KEY_UP: north = current_time; break;
		case ALLEGRO_KEY_DOWN: south = current_time; break;
		}
	}

	void KeyUp (int k) {
		switch (k) {
		case ALLEGRO_KEY_UP: north = 0; break;
		case ALLEGRO_KEY_DOWN: south = 0; break;
		}
	}

	void SimTick () {
		current_time++;
		
		if (north > south) centre_y--;
		if (south > north) centre_y++;
		if (west > east) centre_x--;
		if (east > west) centre_x++;

		scenario->SimTick ();
	}

	void Display (ALLEGRO_DISPLAY * display) {
		al_set_target_backbuffer (display);

		// Correct zoom factor, if needed
		float pix_across = al_get_display_width (display) / zoom;
		if (pix_across > scenario->get_map_width ()) {
			// If scaled map is smaller than display width, change zoom to fit
			pix_across = scenario->get_map_width ();
			zoom = al_get_display_width (display) / pix_across;
		}
		float pix_vertical = al_get_display_height (display) / zoom;
		if (pix_vertical > scenario->get_map_height ()) {
			// If scaled map is smaller than display height, change zoom to fit
			pix_vertical = scenario->get_map_height ();
			zoom = al_get_display_height (display) / pix_vertical;
			pix_across = al_get_display_width (display) / zoom;  // recompute
		}

		// Correct centre position, if needed
		float left_corner_x = centre_x - pix_across / 2;
		if (left_corner_x < 0) {
			centre_x -= left_corner_x;
			left_corner_x = 0;
		}
		float left_corner_y = centre_y - pix_vertical / 2;
		if (left_corner_y < 0) {
			centre_y -= left_corner_y;
			left_corner_y = 0;
		}
		float right_corner_x = centre_x + pix_across / 2;
		if (right_corner_x > scenario->get_map_width() - 1) {
			right_corner_x = scenario->get_map_width() - 1;
			centre_x = right_corner_x - pix_across / 2;
			left_corner_x = centre_x - pix_across / 2;  // recompute
		}
		float right_corner_y = centre_y + pix_vertical / 2;
		if (right_corner_y > scenario->get_map_height() - 1) {
			right_corner_y = scenario->get_map_height() - 1;
			centre_y = right_corner_y - pix_vertical / 2;
			left_corner_y = centre_y - pix_vertical / 2;  // recompute
		}

		// Build transformation matrix for Allegro
		ALLEGRO_TRANSFORM T;
		al_identity_transform (&T);
		al_translate_transform (&T, -left_corner_x, -left_corner_y);
		al_scale_transform (&T, zoom, zoom);
		al_use_transform (&T);

		scenario->Display (al_get_backbuffer (display));
	}

};

#endif
