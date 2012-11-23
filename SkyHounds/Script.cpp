#include "libraries.h"

#include "Script.h"

Script::Script () {
}

Script::Script (std::string _file_name) {
	file_name = _file_name;

	// Open script file
	FILE * fp = fopen (_file_name.c_str(), "rb");
	if (!fp) {
		warning (this, "Could not open script file %s", _file_name.c_str());
		breakpoint ();
		return;
	}

	// Read all lines
	int line_no = 1;
	while (!feof (fp)) {
		char input[2000] = "\0";
		fgets (input, 2000, fp);

		// Find beginning, end, and position of '=' in input
		int first_non_whitespace = -1;
		int last_non_whitespace = -1;
		int equals_position = -1;
		int last_non_whitespace_before_equals = -1;
		int first_non_whitespace_after_equals = -1;
		int i;
		for (i = 0; input[i] != 0; i++) {
			if ('=' == input[i] && equals_position < 0) {
				equals_position = i;
				last_non_whitespace_before_equals = last_non_whitespace;
			}
			else if (input[i] > 32) {
				last_non_whitespace = i;
				if (first_non_whitespace < 0)
					first_non_whitespace = i;
				if (equals_position >= 0 && first_non_whitespace_after_equals < 0)
					first_non_whitespace_after_equals = i;
			}
		}

		// If this line is not empty, extract left and right expressions
		if (first_non_whitespace >= 0) {
			if (equals_position < 0) {
				warning (this, "No '=' on line %d, file %s", line_no, _file_name);
				breakpoint ();
			} else if (last_non_whitespace_before_equals < 0) {
				warning (this, "No left-hand expression on line %d, file %s", line_no, _file_name);
				breakpoint ();
			} else if (first_non_whitespace_after_equals < 0) {
				warning (this, "No right-hand expression on line %d, file %s", line_no, _file_name);
				breakpoint ();
			} else {
				input[last_non_whitespace + 1] = '\0';
				input[last_non_whitespace_before_equals + 1] = '\0';
				std::string left_hand = input + first_non_whitespace;
				std::string right_hand = input + first_non_whitespace_after_equals;
				definitions[left_hand] = right_hand;
			}
		}

		line_no++;  // next line!
	}

	if (definitions.size() == 0) {
		warning (this, "No definitions extracted from script file %s", _file_name);
	}

	// Close script file
	fclose (fp);
}

