#ifndef SYMBOLS_H
#define SYMBOLS_H

#include "SpecialString.h"

#include <iostream>
#include <map>
#include <vector>

class Symbols {

	enum TokenType {
		LETTERS,  // text is a sequence of letters, numbers, and character symbols
		NUMBERS,  // text is a sequence of digits, possibly including decimal punctuation
		SPACING,  // text is a sequence of spaces
		OTHER,    // text is a single character
		NBAR,     // --, nbar, or equivalent
		MBAR,     // ---, mbar, or equivalent
		LONGBAR   // ---- or longer chain of hyphens
	};

	struct Token {
		String text;      // segment of original program text (after double-slash character substitutions)
		String interp;    // interpreted equivalent text
		String language;  // language, if applicable
		TokenType type;
		int line;         // line of source code
	};

	//void tokenize (const String & input, std::vector<Token> & tokens);

	void parse (const String & name, const String & input);

public:

	Symbols ();

	void AddText (String name, const String & text, String * preproc_out = 0);

	void AddTextFromFile (String file_name, String preproc_out_file_name = "");

	static void WriteCharacterReferenceHtml (std::ostream & output);
};

#endif
