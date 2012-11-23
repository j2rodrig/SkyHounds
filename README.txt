Technical Notes

-----
Images/sounds/music are stored in Dropbox. Put the location in SkyHounds\local_options.txt, e.g.:

  dropbox = C:\Users\jonathan\Dropbox\SkyHounds

-----

Map backgrounds are stored in scenarios\MAP_NAME\background.png in Dropbox folder.

-----

SkyHounds\options.txt holds startup configuration. E.g., to set initial scenario to load:

  initial_scenario = standard

-----

The warning and breakpoint functions are designed for notifying us of problems. E.g.:

	// Open script file
	FILE * fp = fopen (_file_name.c_str(), "rb");
	if (!fp) {
		warning (this, "Could not open script file %s", _file_name.c_str());
		breakpoint ();
		return;
	}

The warning function (in errors.h) outputs the message, file name, and line number
to stderr and the MSVC console output window.

The breakpoint function (in porting.h) causes the debugger to pause execution.

The idea here is to catch possible error conditions as soon as they occur
so we don't spend too much time trying to track down bugs.

-----

#include "libraries.h" should be the first line of code in all cpp files.
It includes all the external headers we are using.

-----

Mouse wheel activates zoom feature.

-----

