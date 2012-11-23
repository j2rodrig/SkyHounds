// Main Program

#include "libraries.h"

#include "Control.h"
#include "Scenario.h"

struct SystemState {
	Scenario * scenario;

	// Control mappings for translating commands on various platforms.
	Control * control;

	ALLEGRO_DISPLAY * display;
	ALLEGRO_EVENT_QUEUE * event_queue;
	ALLEGRO_TIMER * frame_timer;
};

static SystemState s_system;

void SystemInitialize ();
void SystemEventLoop ();
void SystemClose ();

int main () {
	SystemInitialize ();

	Script options ("options.txt");
	std::string initial_scenario = options.text ("initial_scenario");

	Script local_options ("local_options.txt");
	std::string dropbox = as_folder (local_options.text ("dropbox"));

	s_system.scenario = new Scenario (initial_scenario, dropbox);
	s_system.control = new Control (s_system.scenario, s_system.display);
	SystemEventLoop ();
	SystemClose ();
	return 0;
}

//----- Function Implementations -----//

void SystemInitialize () {
	if (! al_init ()) {
		breakpoint ();
		exit (-1);
	}

	if (! al_init_image_addon ()) {
		breakpoint ();
		exit (-1);
	}

	// Event Queue
	s_system.event_queue = al_create_event_queue();
	if (! s_system.event_queue) {
		breakpoint ();
		exit (-1);
	}

	// Mouse
	if (! al_install_mouse ()) {
		breakpoint ();
		exit (-1);
	}
	al_register_event_source (s_system.event_queue,
		al_get_mouse_event_source ());

	// Keyboard
	if (! al_install_keyboard ()) {
		breakpoint ();
		exit (-1);
	}
	al_register_event_source (s_system.event_queue,
		al_get_keyboard_event_source ());

	// Display
	al_set_new_display_flags (ALLEGRO_WINDOWED);
	s_system.display = al_create_display (640, 480);
	if (! s_system.display) {
		breakpoint ();
		exit (-1);
	}
	al_register_event_source (s_system.event_queue,
		al_get_display_event_source (s_system.display));

	// Frame Timer
	s_system.frame_timer = al_create_timer (1.0 / 60.0);
	if(! s_system.frame_timer) {
		breakpoint ();
		exit (-1);
	}
	al_register_event_source (s_system.event_queue,
		al_get_timer_event_source (s_system.frame_timer));
	al_start_timer (s_system.frame_timer);
}

void SystemEventLoop () {
	bool redraw = true;
	for (;;) {
		ALLEGRO_EVENT ev;
		al_wait_for_event(s_system.event_queue, &ev);

		// Window closed?
		if (ALLEGRO_EVENT_DISPLAY_CLOSE == ev.type)
			return;

		// Timer event.
		else if (ALLEGRO_EVENT_TIMER == ev.type) {
			// Frame timer. Step physics, flag display redraw.
			if (ev.timer.source == s_system.frame_timer) {
				s_system.control->SimTick ();
				redraw = true;
			}
		}

		// Mouse movement.
		else if (ALLEGRO_EVENT_MOUSE_AXES == ev.type) {
			if (ev.mouse.dz > 0)
				s_system.control->MouseScrollUp (ev.mouse.dz);
			if (ev.mouse.dz < 0)
				s_system.control->MouseScrollDown (-ev.mouse.dz);
			s_system.control->Mouse (ev.mouse.x, ev.mouse.y);
		}

		// Mouse button press.
		else if (ALLEGRO_EVENT_MOUSE_BUTTON_DOWN == ev.type) {
			s_system.control->MouseButton (ev.mouse.button);
		}

		// Mouse left display area.
		else if (ALLEGRO_EVENT_MOUSE_LEAVE_DISPLAY == ev.type) {
			s_system.control->LostMouse ();
		}

		// Mouse entered display area.
		else if (ALLEGRO_EVENT_MOUSE_ENTER_DISPLAY == ev.type) {
			s_system.control->GotMouse ();
		}

		// Character input.
		else if (ALLEGRO_EVENT_KEY_CHAR == ev.type) {
			int c = ev.keyboard.unichar;
			if (c < 32 || c == 127)
				s_system.control->ControlCharacter (c);
			else
				s_system.control->Character (c);
		}

		// Key press (non-repeating).
		else if (ALLEGRO_EVENT_KEY_DOWN == ev.type) {
			// Quit on Escape.
			if (ALLEGRO_KEY_ESCAPE == ev.keyboard.keycode)
				return;
			// Other key press.
			s_system.control->KeyDown (ev.keyboard.keycode);
		}

		// Key release (non-repeating).
		else if (ALLEGRO_EVENT_KEY_UP == ev.type) {
			s_system.control->KeyUp (ev.keyboard.keycode);
		}

		// Draw next frame.
		if (redraw && al_is_event_queue_empty (s_system.event_queue)) {
			s_system.control->Display (s_system.display);
			al_flip_display ();
			redraw = false;
		}
	}
}

void SystemClose () {
	delete s_system.control;
	delete s_system.scenario;
	al_destroy_timer (s_system.frame_timer);
	al_destroy_display (s_system.display);
	al_destroy_event_queue (s_system.event_queue);
}
