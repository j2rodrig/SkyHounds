#ifndef SCRIPT_H
#define SCRIPT_H

class Script {

	std::map<std::string,std::string> definitions;

	std::string file_name;  // just in case we need to know where this script came from

public:

	Script ();  // empty script
	Script (std::string _file_name);  // load from file

	std::string file () const { return file_name; }

	// Add a new definition to this script
	void define (std::string word, std::string def) {
		if (definitions.count(word) > 0) {
			if (definitions[word] != def)
				warning (this,
					"Definition of '%s' in script file %s if being changed to %s",
					word.c_str(), def.c_str(), file_name.c_str());
		}
		definitions[word] = def;
	}

	// Get the definition of a word
	std::string text (std::string word) {
		if (definitions.count(word) == 0) {
			warning (this, "Word '%s' not defined in script file %s",
				word.c_str(), file_name.c_str());
			breakpoint ();
			return "---";
		}
		return definitions[word];
	}

	// Get the definition of a word, with insistance of being an integer
	int integer (std::string word) {
		std::string def = text (word);
		if (def == "---")
			return 0;
		int result = 0;
		int i;
		for (i = 0; i < (signed) def.length(); i++) {
			if (isdigit (def[i]))
				result = result * 10 + def[i] - '0';
			else {
				warning (this,
					"Definition '%s = %s' in script %s does not yield a valid integer",
					word.c_str(), def.c_str(), file_name.c_str());
				breakpoint ();
				break;
			}
		}
		return result;
	}
};

#endif

