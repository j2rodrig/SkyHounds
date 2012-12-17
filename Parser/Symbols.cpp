#include "Symbols.h"

#include "SpecialString.h"

#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

#include <intrin.h>
#define breakpoint()  __debugbreak()

// NOTES:
// The single-quote (') should never be used for character literals.
// It is overloaded for use in symbols (e.g., primes) and ascii character renderings.

//----------- CHARACTERS

#define EMPTY   ""
#define WORD  "WORD"   // may be part of a "word"-token
#define LETTER  "LETTER"   // consecutive strings of letters may not be broken
#define DIGIT  "DIGIT"   // consecutive strings of digits may not be broken
#define HYPHEN  "HYPHEN"  // joins word chunks
#define PUNCTUATION  "PUNCTUATION"   // normally generates word breaks
#define FULLSTOP  "FULLSTOP"
#define COMMA  "COMMA"
#define MIDDOT  "MIDDOT"
#define COLON  "COLON"
#define SEMICOLON  "SEMICOLON"
#define VERTICAL_BAR  "VERTICAL_BAR"
#define CHARACTER_NBAR  "CHARACTER_NBAR"   // equivalent to --
#define CHARACTER_MBAR  "CHARACTER_MBAR"   // equivalent to ---
#define SPACE  "SPACE"
#define NEW_LINE  "NEW_LINE"
#define FORMAT   "FORMAT"  // formatting code: usually ignored
#define QUOTE   "QUOTE"
#define STRUCTURAL  "STRUCTURAL"
#define APOSTROPHE  "APOSTROPHE"
#define OPEN_BLOCK  "OPEN_BLOCK"
#define CLOSE_BLOCK  "CLOSE_BLOCK"
#define DISALLOWED_WITHIN_QUOTE  "DISALLOWED_WITHIN_QUOTE"
#define DISALLOWED_OUTSIDE_QUOTE  "DISALLOWED_OUTSIDE_QUOTE"

class Characters {
protected:
	std::map<String,String> character_equivalence;
	std::multimap<String,String> inverse_character_equivalence;
	std::multimap<String,String> character_characteristics;
	std::multimap<String,String> inverse_character_characteristics;
	std::multimap<String,String> character_codes;
	std::map<String,String> inverse_character_codes;
	std::map<String,String> character_language;

	std::multimap<String,String> block_closers;

	String active_language;
	std::set<String> active_characteristics;

public:
	Characters ();

	void set_equivalent (String equiv, String non_standard) {
		if (character_equivalence.count(non_standard) > 0) {
			breakpoint ();  // Error: Duplicate character
		}
		character_equivalence[non_standard] = equiv;
		inverse_character_equivalence.insert (std::make_pair (equiv, non_standard));
	}
	bool has_equivalent (String non_standard) {
		return character_equivalence.count(non_standard) > 0;
	}
	String get_equivalent (String non_standard) {
		if (character_equivalence.count(non_standard) > 0)
			return character_equivalence[non_standard];
		else
			return non_standard;
	}

	bool is_valid (String non_standard) {
		// A character is considered valid if it has a known code or characteristic.
		return character_codes.count (non_standard) > 0 ||
			character_characteristics.count (non_standard) > 0;
	}

	void add_code (String non_standard, String code) {
		// Adds a single code for a single character.
		character_codes.insert (std::make_pair (non_standard, code));
		if (inverse_character_codes.count(code) > 0) {
			breakpoint ();  // Error: duplicate character code
		}
		inverse_character_codes.insert (std::make_pair (code, non_standard));
	}
	void add_codes (String non_standard, String c1,
			String c2 = EMPTY, String c3 = EMPTY, String c4 = EMPTY) {
		// Adds a multiple codes for a single character.
		if (c1 != EMPTY) add_code (non_standard, c1);
		if (c2 != EMPTY) add_code (non_standard, c2);
		if (c3 != EMPTY) add_code (non_standard, c3);
		if (c4 != EMPTY) add_code (non_standard, c4);
	}
	std::set<String> get_codes (String non_standard) {
		// Retrieves character codes for a given character.
		std::set<String> codes;
		std::multimap<String,String>::iterator i;
		for (i = character_codes.lower_bound(non_standard);
			 i != character_codes.upper_bound(non_standard); i++) {
			codes.insert (i->second);
		}
		return codes;
	}
	bool may_start_code (String starting) {
		// Returns true iff the string may start a character code.
		std::map<String,String>::iterator i;
		i = inverse_character_codes.lower_bound (starting);
		if (i != inverse_character_codes.end())
			return i->first.starts_with_exactly (starting);
		else
			return false;
	}
	String get_codes_string (String non_standard, String delimiter) {
		// Returns codes as a string delimited by delimiter.
		std::set<String> codes = get_codes (non_standard);
		String result = EMPTY;
		std::set<String>::iterator i;
		for (i = codes.begin(); i != codes.end(); i++) {
			result += *i + delimiter;
		}
		return result;
	}

	void add_characteristic (String non_standard, String characteristic) {
		// Adds a single characteristic to a single character.
		character_characteristics.insert (std::make_pair (non_standard, characteristic));
		inverse_character_characteristics.insert (std::make_pair (characteristic, non_standard));
	}
	void add_characteristics (String non_standard, String c1,
			String c2 = EMPTY, String c3 = EMPTY, String c4 = EMPTY) {
		// Adds a multiple characteristics to a single character.
		add_characteristic (non_standard, c1);
		if (c2 != EMPTY) add_characteristic (non_standard, c2);
		if (c3 != EMPTY) add_characteristic (non_standard, c3);
		if (c4 != EMPTY) add_characteristic (non_standard, c4);
	}
	void set_characteristics (String c1, String c2 = EMPTY,
			String c3 = EMPTY, String c4 = EMPTY, String c5 = EMPTY) {
		// Sets default characteristics for subsequent character additions.
		active_characteristics.clear ();
		if (c1 != EMPTY) active_characteristics.insert (c1);
		if (c2 != EMPTY) active_characteristics.insert (c2);
		if (c3 != EMPTY) active_characteristics.insert (c3);
		if (c4 != EMPTY) active_characteristics.insert (c4);
		if (c5 != EMPTY) active_characteristics.insert (c5);
	}
	bool has_characteristic (String non_standard, String characteristic) {
		std::set<String> characteristics = get_characteristics (non_standard);
		return characteristics.count (characteristic) > 0;
	}
	std::set<String> get_characteristics (String non_standard) {
		// Retrieves character characteristics.
		// Characteristics are merged with equivalent character characteristics.
		std::set<String> characteristics;
		std::multimap<String,String>::iterator i;
		for (i = character_characteristics.lower_bound(non_standard);
			 i != character_characteristics.upper_bound(non_standard); i++) {
			characteristics.insert (i->second);
		}
		// Merge equivalent character characteristics.
		String equiv = get_equivalent (non_standard);
		for (i = character_characteristics.lower_bound(equiv);
			 i != character_characteristics.upper_bound(equiv); i++) {
			characteristics.insert (i->second);
		}
		return characteristics;
	}
	String get_characteristics_string (
			String non_standard, String delimiter) {
		// Returns characteristics as a string delimited by delimiter.
		std::set<String> characteristics = get_characteristics (non_standard);
		String result = EMPTY;
		std::set<String>::iterator i;
		for (i = characteristics.begin(); i != characteristics.end(); i++) {
			result += *i + delimiter;
		}
		return result;
	}

	void set_language (String non_standard, String language) {
		// Sets the language of a single character.
		if (language != EMPTY)
			character_language[non_standard] = language;
	}
	void set_language (String language) {
		// Sets default language for subsequent character additions
		active_language = language;
	}
	String get_language (String non_standard) {
		if (character_language.count(non_standard) > 0)
			return character_language[non_standard];
		else
			return EMPTY;
	}

	std::set<String> get_possible_languages (String non_standard) {
		std::set<String> languages;
		// If the character has no equivalent, add its language to the set.
		if (character_equivalence.count(non_standard) == 0) {
			if (character_language.count(non_standard) > 0)
				languages.insert (character_language[non_standard]);
		} else {
			// Else, search all equivalent characters, adding their languages to the set.
			String equiv = character_equivalence[non_standard];
			std::multimap<String,String>::iterator i;
			for (i = inverse_character_equivalence.lower_bound(equiv);
				 i != inverse_character_equivalence.upper_bound(equiv); i++) {
				if (character_language.count(i->second) > 0)
					languages.insert (character_language[i->second]);
			}
		}
		return languages;
	}

	void add (String non_standard, String c1 = EMPTY,
			String c2 = EMPTY, String c3 = EMPTY, String c4 = EMPTY) {
		if (active_language != EMPTY)
			set_language (non_standard, active_language);
		std::set<String>::iterator i;
		for (i = active_characteristics.begin(); i != active_characteristics.end(); i++) {
			add_characteristic (non_standard, *i);
		}
		if (c1 == EMPTY)  // Make sure a code entry exists.
			c1 = non_standard;
		add_codes (non_standard, c1, c2, c3, c4);
	}

	void block_pair (String opener, String closer) {
		// Add a structural pairing
		block_closers.insert (std::make_pair (opener, closer));
	}
	bool is_pairing (String opener, String closer) {
		// Find out if the structural pair (opener, closer) is valid
		std::multimap<String,String>::iterator i;
		for (i = block_closers.lower_bound(opener); i != block_closers.upper_bound(opener); i++) {
			if (i->second == closer)
				return true;
		}
		return false;
	}
	String get_default_pairing (String opener) const {
		// Return the first structural closer that pairs with the given opener
		if (block_closers.count (opener) > 0)
			return block_closers.lower_bound (opener)->second;
		return EMPTY;
	}

	String get_next_character (const String & input, size_t i) const {
		// Any more input?
		if (i >= input.size())
			return EMPTY;

		// Test first byte to find out how long this character is.
		size_t bytes;
		char c = input[i];
		if (c <= 0x7f) {  // ASCII
			if (c == 0x0d && i+1 < input.size() && input[i+1] == 0x0a) // Special case: newline as CRLF
				return "\x0d\x0a";
			else
				bytes = 1;
		}
		else if (c <= 0xbf) bytes = 1;  // Extended/Windows or invalid character
		else if (c <= 0xdf) bytes = 2;
		else if (c <= 0xef) bytes = 3;
		else if (c <= 0xf7) bytes = 4;
		else if (c <= 0xfb) bytes = 5;
		else if (c <= 0xfd) bytes = 6;
		else bytes = 1;   // invalid character, but return it anyway

		// Build return string.
		// Copy bytes, but quit early if end of input or invalid unicode byte reached.
		String result (input[i]);
		size_t j;
		for (j = i + 1; j < i + bytes && j < input.size() && (input[j] & 0xc0) == 0x80; j++) {
			result += input[j];
		}
		return result;
	}

	unsigned char base_16_digit (unsigned char c) {
		if (c < 10)
			return '0' + c;
		else
			return 'a' + (c - 10);
	}

	String byte_string (String c) {
		// Converts a sequence of bytes into human-readable base-16 digits.
		String s;
		int i;
		for (i = 0; i < c.size(); i++) {
			s += base_16_digit ((unsigned)c[i] >> 4);
			s += base_16_digit ((c[i] & 0x0f));
		}
		return s;
	}

	void LexerPass1 (const String & in, String * out) {
		// This pass scans input text backward, looking for character codes
		// delimited by two backslashes. ( E.g., \``\ )
		out->clear ();
		int i = (signed) in.size() - 1;  // characters from i to j (inclusive) have NOT yet been copied.
		int j = (signed) in.size() - 1;

		for (; i >= 0; i--) {
			// If backslash found, it may be the start or end of a character code.
			if (in[i] == '\\') {
				String text = in.slice (i, j);

				// If in[j] is also a backslash, the range i..j (inclusive) may be a code.
				// Is this range a character code?
				if (in[j] == '\\' && inverse_character_codes.count (text) != 0) {
					// Yes: Insert code.
					out->prepend (inverse_character_codes[text]);
					j = i - 1;  // consume entire code.
				} else {
					// No: Copy text verbatim.
					out->insert (out->begin(), text.begin() + 1, text.end());
					j = i;  // consume all text except starting backslash
				}
			}
		}
		// Copy remainder of input.
		if (j >= 0) {
			out->insert (out->begin(), in.begin(), in.begin() + j + 1);
		}
	}
	
	void WriteReferenceHtml (std::ostream & output);

	void WriteQuotesReference (std::ostream & output);

	void WriteLettersReference (std::ostream & output);
};

static Characters characters;

void Characters::WriteReferenceHtml (std::ostream & output) {
	// HEADER
	output <<
			"<!doctype html>\n"
			"<html>\n"
			"<head>\n"
			"<meta http-equiv='Content-Type' content='text/html; charset=UTF-8' />\n"
			"<style type='text/css'>\n"
			"body { font-family: \"Bookman Old Style\", \"Serifa BT\", \"URW Bookman L\", \"itc bookman\", Times, serif; }\n"
			"th { padding-right: 4px; padding-left: 4px; text-align: center; }\n"
			"td { padding-right: 4px; padding-left: 4px; text-align: center; }\n"
			"</style>\n"
			""
			"<title>Character Set Reference</title>\n"
			""
			"</head>\n"
			"<body>\n";

	output <<
		"<h1>Lexical Conventions for an International Programming Language</h1>"
		"The purpose of this effort is to create a coherent and easily-understood "
		"set of lexical conventions for programming in a Unicode-aware environment.\n"
		"A programming language is language that must be interpretable by both "
		"human and machine.\n"
		"Ideally, a programming language will adhere to dominant natural-language "
		"conventions to the fullest extent reasonable "
		"while retaining the ability to expresses necessary concepts and structure "
		"unambiguously.\n"
		<< std::endl;

		output <<
			"<p>\n"
			"<table>\n"
			"<tr><th>Basic Convention</th><th>Explanation</th></tr>\n"
			"<tr><td>(\xe2\x80\x89Group of Things Type 1\xe2\x80\x89)</td><td></td></tr>\n"  // e.g.: subexpression, parameter list
			"<tr><td>{\xe2\x80\x89Group of Things Type 2\xe2\x80\x89}</td><td></td></tr>\n"  // e.g.: set, code block
			"<tr><td>[Comment for the Machine]</td><td>or, Annotation. Sometimes the Machine needs help interpreting the meaning of something.</td></tr>\n"
			"<tr><td>-- Comment for Humans</td><td>Humans need help with interpreting meanings too. (Or just disabling sections of code.)</td></tr>\n"
			"<tr><td>--- Documentation for Humans ---</td><td>Reference Material!</td></tr>\n"
			"<tr><td>A/B</td><td>B below A</td></tr>\n"
			"<tr><td>A\\B</td><td>B above A</td></tr>\n"
			"<tr><td>A-B</td><td>A and B together constitute a single thing</td></tr>\n"
			"<tr><td>\\code\\</td><td>Special Character Code</td></tr>\n"
			"<tr><td>"
				"\xe2\x80\x9cQuoted Literal\xe2\x80\x9d<br />\n"
				"\xe2\x80\x9eQuoted Literal\xe2\x80\x9d<br />\n"
				"\xe2\x80\x9eQuoted Literal\xe2\x80\x9c<br />\n"
				"\xc2\xab\xe2\x80\x89Quoted Literal\xe2\x80\x89\xc2\xbb<br />\n"
				"\xe2\x80\x98Quoted Literal\xe2\x80\x99<br />\n"
				"\xe3\x80\x8cQuoted Literal\xe3\x80\x8d<br />\n"
				"\xe3\x80\x8eQuoted Literal\xe3\x80\x8f<br />\n"
				"</td><td>Literal Quotation, nestable marks</td></tr>\n"
			"<tr><td>\"Quoted Literal\"</td><td>Literal Quotation, legacy marks</td></tr>\n"
			"</table>\n"
			<< std::endl;

	output << "<p>\n"
		<< "<h3>Character Equivalents</h3>"
		<< std::endl;

	WriteLettersReference (output);
	WriteQuotesReference (output);

	// FOOTER
	output <<
			"</body>\n"
			"</html>\n"
			<< std::endl;
}

void Characters::WriteLettersReference (std::ostream & output) {
	output << "<h3>Letters</h3>\n";
	output << "Letters are characters associated with a specific natural language.\n";
	output << "";
	output << std::endl;
}

void Characters::WriteQuotesReference (std::ostream & output) {
	output << "<h3>Quotes</h3>\n";

	output << "Quotation marks delimit quoted literals.\n";
	output << "<ul>\n";
	output << "<li>Quotations can be nested.</li>";
	output << "<li>Outermost quotation marks can be localized.</li>";
	output << "</ul>\n";

	output << "<p>\n";
	output << "The quote table below lists recognized quotation marks, ";
	output << "whether the marks can be the outermost marks in a quote literal, ";
	output << "and whether the marks may appear unescaped inside a quote literal.";

	output << "<p>\n";
	output << "<table>\n";
	output << "<tr><th colspan=2>Quote marks</th>"
		"<th>May be outermost</th><th>Unescaped within quote</th></tr>\n";
	std::multimap<String,String>::iterator i;
	for (i = block_closers.begin(); i != block_closers.end(); i++) {
		if (has_characteristic (i->first, QUOTE)) {
			output << "<tr>";
			output << "<td>" << i->first << "</td>";
			output << "<td>" << i->second << "</td>";
			if (has_characteristic (i->first, DISALLOWED_OUTSIDE_QUOTE))
				output << "<td>No</td>";
			else
				output << "<td>Yes</td>";
			if (has_characteristic (i->first, DISALLOWED_WITHIN_QUOTE))
				output << "<td>No</td>";
			else
				output << "<td>Yes</td>";
			output << "</tr>\n";
		}
	}
	output << "</table>";

	output << "<p>\n";
	output << "There are certain limitations of the usage of various quote characters.\n";
	output << "The ASCII apostrophe (') is never interpreted as a quote delimiter, ";
	output << "due to its already overloaded usage as an apostrophe, a prime mark, and an acute accent.\n";
	output << "The ASCII apostrophe may appear within a quote, but no structural pairing rules are applied to it.\n";
	output << "The ASCII double-quote mark (\") may not appear unescaped within a quote literal. ";
	output << "The single-bottom quote mark (\xe2\x80\x9a) is not permitted to begin a ";
	output << "quote literal due to its visual similarity to the comma (,). ";
	output << "It may, however, be used to form nested sub-quotes.\n";
	output << "The single-angle quotation marks ( \xe2\x80\xb9 ... \xe2\x80\xba ) are ";
	output << "also not permitted to be outermost quote marks, although they may ";
	output << "form nested sub-quotes.\n";
	output << "Outside of quotations, these marks are reserved for programming-language-specific use.\n";
	output << "(One use is to provide a third type of grouping in addition to parentheses and braces, ";
	output << " and another is for XML-like markup tags.)\n";
	output << "Finnish and Swedish unidirectional quotation styles (\xe2\x80\x9d ... \xe2\x80\x9d, \xe2\x80\x99 ... \xe2\x80\x99, ";
	output << "and \xc2\xbb ... \xc2\xbb) are not supported, nor are reversed quotation ";
	output << "styles (\xc2\xbb ... \xc2\xab).\n";
	output << "These styles may be present within a quoted literal, provided that ";
	output << "the quote marks are escaped, and also that the outermost quotation marks use a pairing from ";
	output << "the table above.\n";
	output << "French-style thin spaces next to quote marks ";
	output << "(\xe2\x80\xaf\xc2\xab\xe2\x80\xaf...\xe2\x80\xaf\xc2\xbb\xe2\x80\xaf) ";
	output << "will be interpreted literally, not as presentational formatting of the source code.\n";
	output << "Editors may allow for spaces to be inserted automatically around quote marks ";
	output << "to improve visual presentation, but (at least for outermost quote marks) ";
	output << "these spaces should never be literally present in the source code.\n";

	output << "<p>\n";
	output << "<h3>Rules for Apostrophes</h3>\n";
	output << "A sequence of ASCII apostrophes (', '', ''') at the end of a word ";
	output << "and outside a quote literal is normally ";
	output << "interpreted as a sequence of prime marks (\xe2\x80\xb2, \xe2\x80\xb3, \xe2\x80\xb4).\n";
	output << "Inside of a quoted literal, or at the beginning or middle ";
	output << "of a word, the ASCII apostrophe is normally rendered as a ";
	output << "typographic apostrophe (\xe2\x80\x99).\n";
	output << "Apostrophes generally cannot delimit the start or end of any quote. ";
	output << "However, since the typographic apostrophe is the same character as ";
	output << "the right-single quote, it must be present to close certain quotes.\n";
	output << "Typographic apostrophes within a single-quoted quote literal may ";
	output << "have to be escaped to prevent early closing of the quote.\n";

	output << std::endl;
}

Characters::Characters () {
	set_language (EMPTY);

	set_characteristics (PUNCTUATION, FULLSTOP);
	add (".");
	set_characteristics (PUNCTUATION, COMMA);
	add (",");
	set_characteristics (PUNCTUATION, MIDDOT);
	add ("\xc2\xb7", "\\middot\\");
	set_characteristics (PUNCTUATION, COLON);
	add (":");
	add ("\xd7\x83", "\\sofpasuq\\");
	set_characteristics (PUNCTUATION, SEMICOLON);
	add (";");
	set_characteristics (PUNCTUATION, VERTICAL_BAR);
	add ("|");
	add ("\xc2\xa6", "\\brvbar\\");

	set_characteristics (WORD, HYPHEN);
	add ("-");
	add ("\xe2\x80\90", "\\hyphen\\");
	add ("\xe2\x80\91", "\\nbhyphen\\");

	set_characteristics (CHARACTER_NBAR);
	add ("\xe2\x80\92", "\\figdash\\");
	add ("\xe2\x80\93", "\\ndash\\");

	set_characteristics (CHARACTER_MBAR);
	add ("\xe2\x80\94", "\\mdash\\");
	add ("\xe2\x80\95", "\\hbar\\");

	set_characteristics (WORD);  // These characters do not necessarily generate word breaks
	add ("!", "\\bang\\");
	add ("#");
	add ("$");
	add ("%", "\\percent\\");
	add ("&", "\\amp\\");
	add ("*", "\\star\\");
	add ("+", "\\plus\\");  // symbolic equivalent names: plus
	add ("/");
	add ("\\");
	add ("<", "\\lt\\");
	add ("=");
	add (">", "\\gt\\");
	add ("?");
	add ("@", "\\at\\");
	add ("^");
	add ("_");
	add ("`");
	add ("~");

	set_characteristics (SPACE);
	add ("\x09", "\\tab\\", "\\t");
	add ("\x20", "\\space\\");
	add ("\xa0", "\\windows-nbsp\\");
	add ("\xc2\xa0", "\\nbsp\\");
	add ("\xe2\x80\x80", "\\enquad\\");
	add ("\xe2\x80\x81", "\\emquad\\");
	add ("\xe2\x80\x82", "\\ensp\\");
	add ("\xe2\x80\x83", "\\emsp\\");
	add ("\xe2\x80\x84", "\\third-emsp\\");
	add ("\xe2\x80\x85", "\\fourth-emsp\\");
	add ("\xe2\x80\x86", "\\sixth-emsp\\");
	add ("\xe2\x80\x87", "\\figsp\\");
	add ("\xe2\x80\x88", "\\punctsp\\");
	add ("\xe2\x80\x89", "\\thinsp\\");
	add ("\xe2\x80\x8a", "\\hairsp\\");

	set_characteristics (NEW_LINE, SPACE);
	// Support for legacy line endings: any of cr, lf, crlf is permissible
	add ("\x0a", "\\newline\\", "\\n");  // by popular demand, \n is supported
	add ("\x0b", "\\vtab\\", "\\verticaltab\\");
	add ("\x0c", "\\formfeed\\");
	add ("\x0d", "\\cr\\");
	add ("\x0d\x0a", "\\crlf\\");
	add ("\xc2\x85", "\\nextline\\");
	add ("\xe2\x80\a8", "\\linesep\\");
	add ("\xe2\x80\a9", "\\parsep\\");

	set_characteristics (WORD, FORMAT);
	add ("\xe2\x80\8b", "\\zwsp\\");
	add ("\xe2\x80\8c", "\\zwnj\\");
	add ("\xe2\x80\8d", "\\zwj\\");
	add ("\xe2\x80\8e", "\\lrm\\");
	add ("\xe2\x80\8f", "\\rlm\\");

	set_characteristics (OPEN_BLOCK, STRUCTURAL);
	add ("(");
	add ("{");
	add ("[");
	set_characteristics (CLOSE_BLOCK, STRUCTURAL);
	add (")");
	add ("}");
	add ("]");

	// NOTES:
	// The CLOSE_BLOCK property should only be applied if it is an error
	// for the character not to match its corresponding opening character.
	// e.g., \`\ and \'\ may have meanings other than closing a quote.

	set_characteristics (APOSTROPHE);
	// ? This section needs some work.
	add ("'", "\\apos-ascii\\");
	add ("\x92", "\\apos-windows\\");
	add ("\xe2\x80\x99", "\\'\\", "\\rsquo\\");
	add ("\xe2\x80\xb2", "\\prime\\");
	add ("\xe2\x80\xb3", "\\double-prime\\", "\\Prime\\");
	add ("\xe2\x80\xb4", "\\triple-prime\\");
	add ("\xe2\x81\x97", "\\quadruple-prime\\");
	//set_interpretation ("", "");

	set_characteristics (QUOTE, OPEN_BLOCK);
	add ("\xc2\xab", "\\<<\\", "\\laquo\\");
	//set_equivalent ("\xc2\xab", "\xe3\x80\x8a");
	block_pair ("\xc2\xab", "\xc2\xbb");
	add ("\xe2\x80\x9c", "\\``\\", "\\ldquo\\");
	add ("\xe2\x80\x9e", "\\,,\\", "\\bdquo\\");
	block_pair ("\xe2\x80\x9c", "\xe2\x80\x9d");
	block_pair ("\xe2\x80\x9e", "\xe2\x80\x9d");
	block_pair ("\xe2\x80\x9e", "\xe2\x80\x9c");
	add ("\xe2\x80\x98", "\\`\\", "\\lsquo\\");
	block_pair ("\xe2\x80\x98","\xe2\x80\x99");
	add ("\xe3\x80\x8a", "\\cjk<<\\");
	block_pair ("\xe3\x80\x8a", "\xe3\x80\x8b");
	add ("\xe3\x80\x8c", "\\quoteL\\");
	block_pair ("\xe3\x80\x8c", "\xe3\x80\x8d");
	add ("\xe3\x80\x8e", "\\whiteL\\");
	block_pair ("\xe3\x80\x8e", "\xe3\x80\x8f");

	set_characteristics (QUOTE, OPEN_BLOCK, DISALLOWED_OUTSIDE_QUOTE);
	add ("\xe2\x80\x9a", "\\,\\", "\\sbquo\\");
	block_pair ("\xe2\x80\x9a", "\xe2\x80\x98");
	block_pair ("\xe2\x80\x9a", "\xe2\x80\x99");
	add ("\xe2\x80\xb9", "\\<\\", "\\lsaquo\\");
	//set_equivalent ("\xe2\x80\xb9", "\xe3\x80\x88");
	block_pair ("\xe2\x80\xb9", "\xe2\x80\xba");
	add ("\xe3\x80\x88", "\\cjk<\\");
	block_pair ("\xe3\x80\x88", "\xe3\x80\x89");

	set_characteristics (QUOTE, OPEN_BLOCK, CLOSE_BLOCK, DISALLOWED_WITHIN_QUOTE);
	add ("\"");
	block_pair ("\"", "\"");

	set_characteristics (QUOTE, CLOSE_BLOCK);
	add ("\xc2\xbb", "\\>>\\", "\\raquo\\");
	//set_equivalent ("\xc2\xbb", "\xe3\x80\x8b");
	add ("\xe2\x80\x9d", "\\''\\", "\\rdquo\\");
	add ("\xe2\x80\xba", "\\>\\", "\\rsaquo\\");
	//set_equivalent ("\xe2\x80\xba", "\xe3\x80\x89");
	add ("\xe3\x80\x89", "\\cjk>\\");
	add ("\xe3\x80\x8b", "\\cjk>>\\");
	add ("\xe3\x80\x8d", "\\quoteLstop\\");
	add ("\xe3\x80\x8f", "\\whiteLstop\\");


	//set_characteristics (CHARACTER_SYMBOL);
	//set_language (EMPTY);
	//add (".", "\\fullstop\\");

	set_language (EMPTY);
	set_characteristics (WORD, DIGIT);
	add ("0");
	add ("1");
	add ("2");
	add ("3");
	add ("4");
	add ("5");
	add ("6");
	add ("7");
	add ("8");
	add ("9");

	set_language ("[lat]");
	set_characteristics (WORD, LETTER);
	add ("A");
	add ("B");
	add ("C");
	add ("D");
	add ("E");
	add ("F");
	add ("G");
	add ("H");
	add ("I");
	add ("J");
	add ("K");
	add ("L");
	add ("M");
	add ("N");
	add ("O");
	add ("P");
	add ("Q");
	add ("R");
	add ("S");
	add ("T");
	add ("U");
	add ("V");
	add ("W");
	add ("X");
	add ("Y");
	add ("Z");

	add ("a");
	add ("b");
	add ("c");
	add ("d");
	add ("e");
	add ("f");
	add ("g");
	add ("h");
	add ("i");
	add ("j");
	add ("k");
	add ("l");
	add ("m");
	add ("n");
	add ("o");
	add ("p");
	add ("q");
	add ("r");
	add ("s");
	add ("t");
	add ("u");
	add ("v");
	add ("w");
	add ("x");
	add ("y");
	add ("z");

	/*
	// Latin (supplemental)
	characters.set_language ("latin");

	characters.add ("\xc2\xb5","\xce\xbc",SYMBOL,"micro");

	characters.add ("\xc3\x80","\xc3\x80",LETTER,"A\\`");
	characters.add ("\xc3\x81","\xc3\x81",LETTER,"A\\'");
	characters.add ("\xc3\x82","\xc3\x82",LETTER,"A\\^");
	characters.add ("\xc3\x83","\xc3\x83",LETTER,"A\\~");
	characters.add ("\xc3\x84","\xc3\x84",LETTER,"A\\..");
	characters.add ("\xc3\x85","\xc3\x85",LETTER,"A\\o");
	characters.add ("\xc3\x86","AE",LETTER,"A\\E");
	characters.add ("\xc3\x87","\xc3\x87",LETTER,"C/,");
	characters.add ("\xc3\x88","\xc3\x88",LETTER,"E\\`");
	characters.add ("\xc3\x89","\xc3\x89",LETTER,"E\\'");
	characters.add ("\xc3\x8a","\xc3\x8a",LETTER,"E\\^");
	characters.add ("\xc3\x8b","\xc3\x8b",LETTER,"E\\..");
	characters.add ("\xc3\x8c","\xc3\x8c",LETTER,"I\\`");
	characters.add ("\xc3\x8d","\xc3\x8d",LETTER,"I\\'");
	characters.add ("\xc3\x8e","\xc3\x8e",LETTER,"I\\^");
	characters.add ("\xc3\x8f","\xc3\x8f",LETTER,"I\\..");
	characters.add ("\xc3\x90","\xc3\x90",LETTER,"ETH");
	characters.add ("\xc3\x91","\xc3\x91",LETTER,"N\\~");
	characters.add ("\xc3\x92","\xc3\x92",LETTER,"O\\`");
	characters.add ("\xc3\x93","\xc3\x93",LETTER,"O\\'");
	characters.add ("\xc3\x94","\xc3\x94",LETTER,"O\\^");
	characters.add ("\xc3\x95","\xc3\x95",LETTER,"O\\~");
	characters.add ("\xc3\x96","\xc3\x96",LETTER,"O\\..");
	characters.add ("\xc3\x97","\xc3\x97",SYMBOL,"times");
	characters.add ("\xc3\x98","\xc3\x98",LETTER,"O/");
	characters.add ("\xc3\x99","\xc3\x99",LETTER,"U\\`");
	characters.add ("\xc3\x9a","\xc3\x9a",LETTER,"U\\'");
	characters.add ("\xc3\x9b","\xc3\x9b",LETTER,"U\\^");
	characters.add ("\xc3\x9c","\xc3\x9c",LETTER,"U\\..");
	characters.add ("\xc3\x9d","\xc3\x9d",LETTER,"Y\\'");
	characters.add ("\xc3\x9e","\xc3\x9e",LETTER,"THORN");
	characters.add ("\xc3\x9f","\xc3\x9f",LETTER,"s\\s");
	characters.add ("\xc3\xa0","\xc3\xa0",LETTER,"a\\`");
	characters.add ("\xc3\xa1","\xc3\xa1",LETTER,"a\\'");
	characters.add ("\xc3\xa2","\xc3\xa2",LETTER,"a\\^");
	characters.add ("\xc3\xa3","\xc3\xa3",LETTER,"a\\~");
	characters.add ("\xc3\xa4","\xc3\xa4",LETTER,"a\\..");
	characters.add ("\xc3\xa5","\xc3\xa5",LETTER,"a\\o");
	characters.add ("\xc3\xa6","ae",LETTER,"a\\e");
	characters.add ("\xc3\xa7","\xc3\xa7",LETTER,"c/,");
	characters.add ("\xc3\xa8","\xc3\xa8",LETTER,"e\\`");
	characters.add ("\xc3\xa9","\xc3\xa9",LETTER,"e\\'");
	characters.add ("\xc3\xaa","\xc3\xaa",LETTER,"e\\^");
	characters.add ("\xc3\xab","\xc3\xab",LETTER,"e\\..");
	characters.add ("\xc3\xac","\xc3\xac",LETTER,"i\\`");
	characters.add ("\xc3\xad","\xc3\xad",LETTER,"i\\'");
	characters.add ("\xc3\xae","\xc3\xae",LETTER,"i\\^");
	characters.add ("\xc3\xaf","\xc3\xaf",LETTER,"i\\..");
	characters.add ("\xc3\xb0","\xc3\xb0",LETTER,"eth");
	characters.add ("\xc3\xb1","\xc3\xb1",LETTER,"n\\~");
	characters.add ("\xc3\xb2","\xc3\xb2",LETTER,"o\\`");
	characters.add ("\xc3\xb3","\xc3\xb3",LETTER,"o\\'");
	characters.add ("\xc3\xb4","\xc3\xb4",LETTER,"o\\^");
	characters.add ("\xc3\xb5","\xc3\xb5",LETTER,"o\\~");
	characters.add ("\xc3\xb6","\xc3\xb6",LETTER,"o\\..");
	characters.add ("\xc3\xb7","\xc3\xb7",SYMBOL,"divide");
	characters.add ("\xc3\xb8","\xc3\xb8",LETTER,"o/");
	characters.add ("\xc3\xb9","\xc3\xb9",LETTER,"u\\`");
	characters.add ("\xc3\xba","\xc3\xba",LETTER,"u\\'");
	characters.add ("\xc3\xbb","\xc3\xbb",LETTER,"u\\^");
	characters.add ("\xc3\xbc","\xc3\xbc",LETTER,"u\\..");
	characters.add ("\xc3\xbd","\xc3\xbd",LETTER,"y\\'");
	characters.add ("\xc3\xbe","\xc3\xbe",LETTER,"thorn");
	characters.add ("\xc3\xbf","\xc3\xbf",LETTER,"y\\..");

	characters.add ("\xc5\x92","OE",LETTER,"O\\E");
	characters.add ("\xc5\x93","oe",LETTER,"o\\e");
	characters.add ("\xc5\xa0","\xc5\xa0",LETTER,"S\\v");
	characters.add ("\xc5\xa1","\xc5\xa1",LETTER,"s\\v");
	characters.add ("\xc5\xb8","\xc5\xb8",LETTER,"Y\\..");

	// Hebrew
	characters.set_language ("hebrew");

	characters.add ("\xd6\xb0","\xd6\xb0",POINTING,"\\sheva\\");
	characters.add ("\xd6\xb1","\xd6\xb1",POINTING,"\\hatafsegol\\");
	characters.add ("\xd6\xb2","\xd6\xb2",POINTING,"\\hatafpatah\\");
	characters.add ("\xd6\xb3","\xd6\xb3",POINTING,"\\hatafqamats\\");
	characters.add ("\xd6\xb4","\xd6\xb4",POINTING,"\\hiriq\\");
	characters.add ("\xd6\xb5","\xd6\xb5",POINTING,"\\tsere\\");
	characters.add ("\xd6\xb6","\xd6\xb6",POINTING,"\\segol\\");
	characters.add ("\xd6\xb7","\xd6\xb7",POINTING,"\\patah\\");
	characters.add ("\xd6\xb8","\xd6\xb8",POINTING,"\\qamats\\");
	characters.add ("\xd6\xb9","\xd6\xb9",POINTING,"\\holam\\");
	characters.add ("\xd6\xba","\xd6\xba",POINTING,"\\holamhaservav\\");
	characters.add ("\xd6\xbb","\xd6\xbb",POINTING,"\\qubuts\\");
	characters.add ("\xd6\xbc","\xd6\xbc",POINTING,"\\dagesh\\","\\mapiq\\");
	characters.add ("\xd6\xbd","\xd6\xbd",POINTING,"\\meteg\\");
	characters.add ("\xd6\xbe","-",SPECIAL,"\\maqaf\\","\\-\\");
	characters.add ("\xd6\xbf","\xd6\xbf",POINTING,"\\rafe\\");
	characters.add ("\xd7\x80","|",PUNCTUATION,"\\paseq\\");
	characters.add ("\xd7\x81","\xd7\x81",POINTING,"\\shindot\\");
	characters.add ("\xd7\x82","\xd7\x82",POINTING,"\\sindot\\");
	characters.add ("\xd7\x84","\xd7\x84",POINTING,"\\upperdot\\");
	characters.add ("\xd7\x85","\xd7\x85",POINTING,"\\lowerdot\\");
	characters.add ("\xd7\x86","\xd7\x86",SYMBOL,"\\nunhafukha\\");
	characters.add ("\xd7\x87","\xd7\x87",POINTING,"\\qamatsqatan\\");

	characters.add ("\xd7\x90","\xd7\x90",LETTER,"alef");
	characters.add ("\xd7\x91","\xd7\x91",LETTER,"bet");
	characters.add ("\xd7\x92","\xd7\x92",LETTER,"gimel");
	characters.add ("\xd7\x93","\xd7\x93",LETTER,"dalet");
	characters.add ("\xd7\x94","\xd7\x94",LETTER,"he");
	characters.add ("\xd7\x95","\xd7\x95",LETTER,"vav");
	characters.add ("\xd7\x96","\xd7\x96",LETTER,"zayin");
	characters.add ("\xd7\x97","\xd7\x97",LETTER,"het");
	characters.add ("\xd7\x98","\xd7\x98",LETTER,"tet");
	characters.add ("\xd7\x99","\xd7\x99",LETTER,"yod");
	characters.add ("\xd7\x9a","\xd7\x9a",LETTER,"finalkaf");
	characters.add ("\xd7\x9b","\xd7\x9b",LETTER,"kaf");
	characters.add ("\xd7\x9c","\xd7\x9c",LETTER,"lamed");
	characters.add ("\xd7\x9d","\xd7\x9d",LETTER,"finalmem");
	characters.add ("\xd7\x9e","\xd7\x9e",LETTER,"mem");
	characters.add ("\xd7\x9f","\xd7\x9f",LETTER,"finalnun");
	characters.add ("\xd7\xa0","\xd7\xa0",LETTER,"nun");
	characters.add ("\xd7\xa1","\xd7\xa1",LETTER,"samekh");
	characters.add ("\xd7\xa2","\xd7\xa2",LETTER,"ayin");
	characters.add ("\xd7\xa3","\xd7\xa3",LETTER,"finalpe");
	characters.add ("\xd7\xa4","\xd7\xa4",LETTER,"pe");
	characters.add ("\xd7\xa5","\xd7\xa5",LETTER,"finaltsadi");
	characters.add ("\xd7\xa6","\xd7\xa6",LETTER,"tsadi");
	characters.add ("\xd7\xa7","\xd7\xa7",LETTER,"qof");
	characters.add ("\xd7\xa8","\xd7\xa8",LETTER,"resh");
	characters.add ("\xd7\xa9","\xd7\xa9",LETTER,"shin");
	characters.add ("\xd7\xaa","\xd7\xaa",LETTER,"tav");

	characters.add ("\xd7\xb0","\xd7\x95\xd7\x95",LETTER,"vav\\vav");
	characters.add ("\xd7\xb1","\xd7\x95\xd7\x99",LETTER,"vav\\yod");
	characters.add ("\xd7\xb2","\xd7\x99\xd7\x99",LETTER,"yod\\yod");
	characters.add ("\xd7\xb3","'",SYMBOL,"\\geresh\\");
	characters.add ("\xd7\xb4","''",SYMBOL,"\\gershayim\\");

	// Greek
	characters.set_language ("greek");

	characters.add ("\xce\x86","\xce\x86",LETTER,"'ALPHA");
	characters.add ("\xce\x87",MIDDOT,SYMBOL,"\\anotelia\\");
	characters.add ("\xce\x88","\xce\x88",LETTER,"'EPSILON");
	characters.add ("\xce\x89","\xce\x89",LETTER,"'ETA");
	characters.add ("\xce\x8a","\xce\x8a",LETTER,"'IOTA");
	characters.add ("\xce\x8c","\xce\x8c",LETTER,"'OMICRON");
	characters.add ("\xce\x8e","\xce\x8e",LETTER,"'UPSILON");
	characters.add ("\xce\x8f","\xce\x8f",LETTER,"'OMEGA");

	characters.add ("\xce\x91","A",LETTER,"ALPHA");
	characters.add ("\xce\x92","B",LETTER,"BETA");
	characters.add ("\xce\x93","\xce\x93",LETTER,"GAMMA");
	characters.add ("\xce\x94","\xce\x94",LETTER,"DELTA");
	characters.add ("\xce\x95","E",LETTER,"EPSILON");
	characters.add ("\xce\x96","Z",LETTER,"ZETA");
	characters.add ("\xce\x97","H",LETTER,"ETA");
	characters.add ("\xce\x98","\xce\x98",LETTER,"THETA");
	characters.add ("\xce\x99","I",LETTER,"IOTA");
	characters.add ("\xce\x9a","K",LETTER,"KAPPA");
	characters.add ("\xce\x9b","\xce\x9b",LETTER,"LAMBDA");
	characters.add ("\xce\x9c","M",LETTER,"MU");
	characters.add ("\xce\x9d","N",LETTER,"NU");
	characters.add ("\xce\x9e","\xce\x9e",LETTER,"XI");
	characters.add ("\xce\x9f","O",LETTER,"OMICRON");
	characters.add ("\xce\xa0","\xce\xa0",LETTER,"PI");
	characters.add ("\xce\xa1","P",LETTER,"RHO");
	characters.add ("\xce\xa3","\xce\xa3",LETTER,"SIGMA");
	characters.add ("\xce\xa4","T",LETTER,"TAU");
	characters.add ("\xce\xa5","Y",LETTER,"UPSILON");
	characters.add ("\xce\xa6","\xce\xa6",LETTER,"PHI");
	characters.add ("\xce\xa7","X",LETTER,"CHI");
	characters.add ("\xce\xa8","\xce\xa8",LETTER,"PSI");
	characters.add ("\xce\xa9","\xce\xa9",LETTER,"OMEGA");
	characters.add ("\xce\xaa","\xc3\x8f",LETTER,"IOTA\\..");
	characters.add ("\xce\xab","\xc5\xb8",LETTER,"UPSILON\\..");
	characters.add ("\xce\xac","\xce\xac",LETTER,"'alpha");
	characters.add ("\xce\xad","\xce\xad",LETTER,"'epsilon");
	characters.add ("\xce\xae","\xce\xae",LETTER,"'eta");
	characters.add ("\xce\xaf","\xce\xaf",LETTER,"'iota");

	characters.add ("\xce\xb1","\xce\xb1",LETTER,"alpha");
	characters.add ("\xce\xb2","\xce\xb2",LETTER,"beta");
	characters.add ("\xce\xb3","\xce\xb3",LETTER,"gamma");
	characters.add ("\xce\xb4","\xce\xb4",LETTER,"delta");
	characters.add ("\xce\xb5","\xce\xb5",LETTER,"epsilon");
	characters.add ("\xce\xb6","\xce\xb6",LETTER,"zeta");
	characters.add ("\xce\xb7","\xce\xb7",LETTER,"eta");
	characters.add ("\xce\xb8","\xce\xb8",LETTER,"theta");
	characters.add ("\xce\xb9","\xce\xb9",LETTER,"iota");
	characters.add ("\xce\xba","\xce\xba",LETTER,"kappa");
	characters.add ("\xce\xbb","\xce\xbb",LETTER,"lambda");
	characters.add ("\xce\xbc","\xce\xbc",LETTER,"mu");
	characters.add ("\xce\xbd","\xce\xbd",LETTER,"nu");
	characters.add ("\xce\xbe","\xce\xbe",LETTER,"xi");
	characters.add ("\xce\xbf","\xce\xbf",LETTER,"omicron");
	characters.add ("\xcf\x80","\xcf\x80",LETTER,"pi");
	characters.add ("\xcf\x81","\xcf\x81",LETTER,"rho");
	characters.add ("\xcf\x82","\xcf\x82",LETTER,"finalsigma");
	characters.add ("\xcf\x83","\xcf\x83",LETTER,"sigma");
	characters.add ("\xcf\x84","\xcf\x84",LETTER,"tau");
	characters.add ("\xcf\x85","\xcf\x85",LETTER,"upsilon");
	characters.add ("\xcf\x86","\xcf\x86",LETTER,"phi");
	characters.add ("\xcf\x87","\xcf\x87",LETTER,"chi");
	characters.add ("\xcf\x88","\xcf\x88",LETTER,"psi");
	characters.add ("\xcf\x89","\xcf\x89",LETTER,"omega");
	characters.add ("\xcf\x8a","\xcf\x8a",LETTER,"iota\\..");
	characters.add ("\xcf\x8b","\xcf\x8b",LETTER,"upsilon\\..");
	characters.add ("\xcf\x8c","\xcf\x8c",LETTER,"'omicron");
	characters.add ("\xcf\x8d","\xcf\x8d",LETTER,"'upsilon");
	characters.add ("\xcf\x8e","\xcf\x8e",LETTER,"'omega");
	characters.add ("\xcf\x8f","\xcf\x8f",LETTER,"KAI");

	characters.add ("\xcf\x91","\xcf\x91",LETTER,"thetasym");
	characters.add ("\xcf\x92","\xcf\x92",LETTER,"upsilonhook","upsilonsym");
	characters.add ("\xcf\x95","\xcf\x95",LETTER,"phisym");
	characters.add ("\xcf\x96","\xcf\x96",LETTER,"pisym");
	characters.add ("\xcf\x97","\xcf\x97",LETTER,"kai");

	// Hiragana
	characters.set_language ("hiragana");

	// Katakana
	characters.set_language ("katakana");

	characters.add ("\xe3\x82\xa0","=",SYMBOL,"\\doublehyphen\\");
	characters.add ("\xe3\x82\xa1","\xe3\x82\xa1",LETTER,"\\kat-a\\");
	characters.add ("\xe3\x82\xa2","\xe3\x82\xa2",LETTER,"A");
	characters.add ("\xe3\x82\xa3","\xe3\x82\xa3",LETTER,"\\kat-i\\");
	characters.add ("\xe3\x82\xa4","\xe3\x82\xa4",LETTER,"I");
	characters.add ("\xe3\x82\xa5","\xe3\x82\xa5",LETTER,"\\kat-u\\");
	characters.add ("\xe3\x82\xa6","\xe3\x82\xa6",LETTER,"U");
	characters.add ("\xe3\x82\xa7","\xe3\x82\xa7",LETTER,"E");
	characters.add ("\xe3\x82\xa8","\xe3\x82\xa8",LETTER,"\\kat-e\\");
	characters.add ("\xe3\x82\xa9","\xe3\x82\xa9",LETTER,"\\kat-o\\");
	characters.add ("\xe3\x82\xaa","\xe3\x82\xaa",LETTER,"O");
	characters.add ("\xe3\x82\xab","\xe3\x82\xab",LETTER,"Ka");
	characters.add ("\xe3\x82\xac","\xe3\x82\xac",LETTER,"Ga");
	characters.add ("\xe3\x82\xad","\xe3\x82\xad",LETTER,"Ki");
	characters.add ("\xe3\x82\xae","\xe3\x82\xae",LETTER,"Gi");
	characters.add ("\xe3\x82\xaf","\xe3\x82\xaf",LETTER,"Ku");
	characters.add ("\xe3\x82\xb0","\xe3\x82\xb0",LETTER,"Gu");
	characters.add ("\xe3\x82\xb1","\xe3\x82\xb1",LETTER,"Ke");
	characters.add ("\xe3\x82\xb2","\xe3\x82\xb2",LETTER,"Ge");
	characters.add ("\xe3\x82\xb3","\xe3\x82\xb3",LETTER,"Ko");
	characters.add ("\xe3\x82\xb4","\xe3\x82\xb4",LETTER,"Go");
	characters.add ("\xe3\x82\xb5","\xe3\x82\xb5",LETTER,"Sa");
	characters.add ("\xe3\x82\xb6","\xe3\x82\xb6",LETTER,"Za");
	characters.add ("\xe3\x82\xb7","\xe3\x82\xb7",LETTER,"Si");
	characters.add ("\xe3\x82\xb8","\xe3\x82\xb8",LETTER,"Zi");
	characters.add ("\xe3\x82\xb9","\xe3\x82\xb9",LETTER,"Su");
	characters.add ("\xe3\x82\xba","\xe3\x82\xba",LETTER,"Zu");
	characters.add ("\xe3\x82\xbb","\xe3\x82\xbb",LETTER,"Se");
	characters.add ("\xe3\x82\xbc","\xe3\x82\xbc",LETTER,"Ze");
	characters.add ("\xe3\x82\xbd","\xe3\x82\xbd",LETTER,"So");
	characters.add ("\xe3\x82\xbe","\xe3\x82\xbe",LETTER,"Zo");
	characters.add ("\xe3\x82\xbf","\xe3\x82\xbf",LETTER,"Ta");
	characters.add ("\xe3\x83\x80","\xe3\x83\x80",LETTER,"Da");
	characters.add ("\xe3\x83\x81","\xe3\x83\x81",LETTER,"Ti");
	characters.add ("\xe3\x83\x82","\xe3\x83\x82",LETTER,"Di");
	characters.add ("\xe3\x83\x83","\xe3\x83\x83",LETTER,"tu");
	characters.add ("\xe3\x83\x84","\xe3\x83\x84",LETTER,"Tu");
	characters.add ("\xe3\x83\x85","\xe3\x83\x85",LETTER,"Du");
	characters.add ("\xe3\x83\x86","\xe3\x83\x86",LETTER,"Te");
	characters.add ("\xe3\x83\x87","\xe3\x83\x87",LETTER,"De");
	characters.add ("\xe3\x83\x88","\xe3\x83\x88",LETTER,"To");
	characters.add ("\xe3\x83\x89","\xe3\x83\x89",LETTER,"Do");
	characters.add ("\xe3\x83\x8a","\xe3\x83\x8a",LETTER,"Na");
	characters.add ("\xe3\x83\x8b","\xe3\x83\x8b",LETTER,"Ni");
	characters.add ("\xe3\x83\x8c","\xe3\x83\x8c",LETTER,"Nu");
	characters.add ("\xe3\x83\x8d","\xe3\x83\x8d",LETTER,"Ne");
	characters.add ("\xe3\x83\x8e","\xe3\x83\x8e",LETTER,"No");
	characters.add ("\xe3\x83\x8f","\xe3\x83\x8f",LETTER,"Ha");
	characters.add ("\xe3\x83\x90","\xe3\x83\x90",LETTER,"Ba");
	characters.add ("\xe3\x83\x91","\xe3\x83\x91",LETTER,"Pa");
	characters.add ("\xe3\x83\x92","\xe3\x83\x92",LETTER,"Hi");
	characters.add ("\xe3\x83\x93","\xe3\x83\x93",LETTER,"Bi");
	characters.add ("\xe3\x83\x94","\xe3\x83\x94",LETTER,"Pi");
	characters.add ("\xe3\x83\x95","\xe3\x83\x95",LETTER,"Hu");
	characters.add ("\xe3\x83\x96","\xe3\x83\x96",LETTER,"Bu");
	characters.add ("\xe3\x83\x97","\xe3\x83\x97",LETTER,"Pu");
	characters.add ("\xe3\x83\x98","\xe3\x83\x98",LETTER,"He");
	characters.add ("\xe3\x83\x99","\xe3\x83\x99",LETTER,"Be");
	characters.add ("\xe3\x83\x9a","\xe3\x83\x9a",LETTER,"Pe");
	characters.add ("\xe3\x83\x9b","\xe3\x83\x9b",LETTER,"Ho");
	characters.add ("\xe3\x83\x9c","\xe3\x83\x9c",LETTER,"Bo");
	characters.add ("\xe3\x83\x9d","\xe3\x83\x9d",LETTER,"Po");
	characters.add ("\xe3\x83\x9e","\xe3\x83\x9e",LETTER,"Ma");
	characters.add ("\xe3\x83\x9f","\xe3\x83\x9f",LETTER,"Mi");
	characters.add ("\xe3\x83\xa0","\xe3\x83\xa0",LETTER,"Mu");
	characters.add ("\xe3\x83\xa1","\xe3\x83\xa1",LETTER,"Me");
	characters.add ("\xe3\x83\xa2","\xe3\x83\xa2",LETTER,"Mo");
	characters.add ("\xe3\x83\xa3","\xe3\x83\xa3",LETTER,"ya");
	characters.add ("\xe3\x83\xa4","\xe3\x83\xa4",LETTER,"Ya");
	characters.add ("\xe3\x83\xa5","\xe3\x83\xa5",LETTER,"yu");
	characters.add ("\xe3\x83\xa6","\xe3\x83\xa6",LETTER,"Yu");
	characters.add ("\xe3\x83\xa7","\xe3\x83\xa7",LETTER,"yo");
	characters.add ("\xe3\x83\xa8","\xe3\x83\xa8",LETTER,"Yo");
	characters.add ("\xe3\x83\xa9","\xe3\x83\xa9",LETTER,"Ra");
	characters.add ("\xe3\x83\xaa","\xe3\x83\xaa",LETTER,"Ri");
	characters.add ("\xe3\x83\xab","\xe3\x83\xab",LETTER,"Ru");
	characters.add ("\xe3\x83\xac","\xe3\x83\xac",LETTER,"Re");
	characters.add ("\xe3\x83\xad","\xe3\x83\xad",LETTER,"Ro");
	characters.add ("\xe3\x83\xae","\xe3\x83\xae",LETTER,"wa");
	characters.add ("\xe3\x83\xaf","\xe3\x83\xaf",LETTER,"Wa");
	characters.add ("\xe3\x83\xb0","\xe3\x83\xb0",LETTER,"Wi");
	characters.add ("\xe3\x83\xb1","\xe3\x83\xb1",LETTER,"We");
	characters.add ("\xe3\x83\xb2","\xe3\x83\xb2",LETTER,"Wo");
	characters.add ("\xe3\x83\xb3","\xe3\x83\xb3",LETTER,"N");
	characters.add ("\xe3\x83\xb4","\xe3\x83\xb4",LETTER,"Vu");
	characters.add ("\xe3\x83\xb5","\xe3\x83\xb5",LETTER,"ka");
	characters.add ("\xe3\x83\xb6","\xe3\x83\xb6",LETTER,"ke");
	characters.add ("\xe3\x83\xb7","\xe3\x83\xb7",LETTER,"Va");
	characters.add ("\xe3\x83\xb8","\xe3\x83\xb8",LETTER,"Vi");
	characters.add ("\xe3\x83\xb9","\xe3\x83\xb9",LETTER,"Ve");
	characters.add ("\xe3\x83\xba","\xe3\x83\xba",LETTER,"Vo");
	characters.add ("\xe3\x83\xbb",MIDDOT,SYMBOL,"\\katmiddot\\");
	characters.add ("\xe3\x83\xbc","\xe3\x83\xbc",SYMBOL,"\\prolongedsound\\");
	characters.add ("\xe3\x83\xbd","\xe3\x83\xbd",SYMBOL,"\\iteration\\");
	characters.add ("\xe3\x83\xbe","\xe3\x83\xbe",SYMBOL,"\\voicediteration\\");

	// General Punctuation
	characters.set_language ("");

	characters.add ("\xe2\x80\x80","\xe2\x80\x80",SPACE,"\\enquad\\");
	characters.add ("\xe2\x80\x81","\xe2\x80\x81",SPACE,"\\emquad\\");
	characters.add ("\xe2\x80\x82","\xe2\x80\x82",SPACE,"\\enspace\\","\\ensp\\");
	characters.add ("\xe2\x80\x83","\xe2\x80\x83",SPACE,"\\emspace\\","\\emsp\\");
	characters.add ("\xe2\x80\x89","\xe2\x80\x89",SPACE,"\\thinspace\\","\\thinsp\\");
	characters.add ("\xe2\x80\x8b","\xe2\x80\x8b",FORMAT_CODE,"\\zwspace\\","\\zwsp\\");
	characters.add ("\xe2\x80\x8c","\xe2\x80\x8c",FORMAT_CODE,"\\zwnj\\");
	characters.add ("\xe2\x80\x8d","\xe2\x80\x8d",FORMAT_CODE,"\\zwj\\");
	characters.add ("\xe2\x80\x8e","\xe2\x80\x8e",FORMAT_CODE,"\\leftright\\","\\lrm\\");
	characters.add ("\xe2\x80\x8f","\xe2\x80\x8f",FORMAT_CODE,"\\rightleft\\","\\rlm\\");
	characters.add ("\xe2\x80\x90","-",SPECIAL,"\\hyphen\\");
	characters.add ("\xe2\x80\x91","-",SPECIAL,"\\nbhyphen\\");
	characters.add ("\xe2\x80\x92","--",SPECIAL,"\\figdash\\");
	characters.add ("\xe2\x80\x93","--",SPECIAL,"\\ndash\\","\\endash\\");
	characters.add ("\xe2\x80\x94","---",SPECIAL,"\\mdash\\","\\emdash\\");
	characters.add ("\xe2\x80\x95","---",SPECIAL,"\\hbar\\");

	characters.add ("\xe2\x80\x98","\xe2\x80\x98",QUOTE,"\\lsquo\\","[`]");
	characters.add ("\xe2\x80\x99","\xe2\x80\x99",QUOTE,"\\rsquo\\","[']");
	characters.add ("\xe2\x80\x9a","\xe2\x80\x9a",QUOTE,"\\sbquo\\","[,]");
	characters.add_structural_pair ("\xe2\x80\x98","\xe2\x80\x99");
	characters.add_structural_pair ("\xe2\x80\x9a","\xe2\x80\x99");
	characters.add_structural_pair ("\xe2\x80\x9a","\xe2\x80\x98");

	characters.add ("\xe2\x80\x9c","\xe2\x80\x9c",QUOTE,"\\ldquo\\","[``]");
	characters.add ("\xe2\x80\x9d","\xe2\x80\x9d",QUOTE,"\\rdquo\\","['']");
	characters.add ("\xe2\x80\x9e","\xe2\x80\x9e",QUOTE,"\\bdquo\\","[,,]");
	characters.add_structural_pair ("\xe2\x80\x9c","\xe2\x80\x9d");
	characters.add_structural_pair ("\xe2\x80\x9e","\xe2\x80\x9d");
	characters.add_structural_pair ("\xe2\x80\x9e","\xe2\x80\x9c");

	characters.add ("\xe2\x80\xa2",MIDDOT,SYMBOL,"\\bullet\\","\\bull\\");

	characters.add ("\xe2\x80\xb9","\xe2\x80\xb9",QUOTE,"\\lsaquo\\","[<]");
	characters.add ("\xe2\x80\xba","\xe2\x80\xba",QUOTE,"\\rsaquo\\","[>]");
	characters.add_structural_pair ("\xe2\x80\xb9","\xe2\x80\xba");

	characters.add ("\xe2\x80\xa6","...",PUNCTUATION,"\\ellipsis\\","\\hellip\\");

	characters.add ("\xe2\x80\xaf","\xe2\x80\xaf",SPACE,"\\nnbsp\\");

	// Chinese, Japanese, Korean Punctuation
	characters.add ("\xe3\x80\x88","<",QUOTE,"\\cjk<\\");
	characters.add ("\xe3\x80\x89",">",QUOTE,"\\cjk>\\");
	characters.add ("\xe3\x80\x8a","\xc2\xab",QUOTE,"\\cjk<<\\");
	characters.add ("\xe3\x80\x8b","\xc2\xbb",QUOTE,"\\cjk>>\\");
	characters.add ("\xe3\x80\x8c","\xe3\x80\x8c",QUOTE,"\\quoteL\\");
	characters.add ("\xe3\x80\x8d","\xe3\x80\x8d",QUOTE,"\\quoteLstop\\");
	characters.add_structural_pair ("\xe3\x80\x8c","\xe3\x80\x8d");
	characters.add ("\xe3\x80\x8e","\xe3\x80\x8e",QUOTE,"\\whiteL\\");
	characters.add ("\xe3\x80\x8f","\xe3\x80\x8f",QUOTE,"\\whiteLstop\\");
	characters.add_structural_pair ("\xe3\x80\x8e","\xe3\x80\x8f");

	//characters.add ("","","","");
	*/
}

//----------- !CHARACTERS

Symbols::Symbols () {
}

void Symbols::AddText (String name, const String & text, String * preproc_out) {
	String preproc;
	characters.LexerPass1 (text, &preproc);
	if (preproc_out)
		(*preproc_out) = preproc;
	parse (name, preproc);
}

void Symbols::AddTextFromFile (String file_name, String preproc_out_file_name) {
	std::ifstream is;
	int size;
	is.open (file_name, std::ios::binary);

	// Find file size
	is.seekg (0, std::ios::end);
	size = (int) is.tellg ();
	is.seekg (0, std::ios::beg);

	// Read file contents
	char * buffer = new char[size];
	is.read (buffer, size);
	String input (buffer, size);  // store in String object
	delete buffer;
	is.close ();

	//
	if (preproc_out_file_name != "") {
		String preproc;
		AddText (file_name, input, &preproc);

		// Write preprocessor output to file.
		std::ofstream out_file (preproc_out_file_name, std::ios::binary);
		out_file.write (preproc.data(), preproc.size());
		out_file.close();
	} else {
		// No preprocessor output desired.
		AddText (file_name, input);
	}
}

//void Symbols::tokenize (const String & input, std::vector<Token> & tokens) {
//	int i = 0;
//	for (;;) {
//	}
//}

// Token types (tokens can also share character types)
#define END_OF_INPUT  "END_OF_INPUT"
#define START_OF_INPUT  "START_OF_INPUT"
#define SPACE_SEQUENCE  "SPACE_SEQUENCE"
#define ROUGH_TOKEN  "ROUGH_TOKEN"

class Parser {
public:
	struct InputStat {
		const String & name;
		const String & text;
		size_t pos;      // current character under consideration
		size_t line_no;  // line number under consideration
		std::vector<String> errors;
		InputStat (const String & _name, const String & _text)
			: name(_name), text(_text), pos(0), line_no(1) {
		}
	};

	struct Token {
		String text;
		String type;
		size_t line_no;
		size_t new_line_count;  // for space sequences, how many newlines are contained
		bool character_literal;  // should be taken literally, e.g. ["]
		bool in_quoted_section;  // if this token is inside a quoted section

		String language_code;

		Token * outer;  // start of outer block (set during preliminary parsing)
		Token (size_t _line_no, bool _in_quoted)
			: line_no(_line_no), new_line_count(0),
			  character_literal(false), in_quoted_section(_in_quoted),
			  outer(NULL) {}
	};

	// Token sequence prior to performing equivalency conversions
	std::vector<Token> rough_tokens;

	// Token sequence after equivalency conversions
	std::vector<Token> clean_tokens;

	String next_character (InputStat & is) {
		String c = characters.get_next_character (is.text, is.pos);
		is.pos += c.size();
		if (characters.has_characteristic (c, NEW_LINE))
			is.line_no++;
		return c;
	}

	String peek_character (InputStat & is, size_t forward = 1) {
		// Looks at a character without advancing input.
		// forward specifies how many characters of look-ahead are desired.
		size_t p = is.pos;
		String c;
		while (forward > 0) {
			c = characters.get_next_character (is.text, p);
			p += c.size();
			forward--;
		}
		return c;
	}

	Token rough_token (InputStat & is, bool in_quoted_section, String open_block_char = EMPTY) {
		// Rough tokens are chunks of text separated by whitespace, block openers, and block closers
		Token token (is.line_no, in_quoted_section);
		String c = next_character (is);

		// End of input?
		if (c == EMPTY) {
			token.type = END_OF_INPUT;
			return token;
		}

		// Non-paired character literal? (in quoted section)
		if (in_quoted_section && c == "[") {
			String p = peek_character (is);
			if (characters.has_characteristic (p, QUOTE) || characters.has_characteristic (p, STRUCTURAL)) {
				if (peek_character (is, 2) == "]") {
					// Single-character annotation. Return literally.
					token.text = next_character (is);
					token.type = ROUGH_TOKEN;
					token.character_literal = true;
					next_character (is);  // consume closing bracket
					return token;
				}
			}
			if (peek_character (is) == "[") {
				// Double-[
				token.text = next_character (is);
				token.type = ROUGH_TOKEN;
				token.character_literal = true;
				return token;
			}
		}
		if (in_quoted_section && c == "]") {
			if (peek_character (is) == "]") {
				// Double-]
				token.text = next_character (is);
				token.type = ROUGH_TOKEN;
				token.character_literal = true;
				return token;
			}
		}

		token.text += c;

		// Check for block openers and matching closers
		if (characters.has_characteristic (c, OPEN_BLOCK)) {
			token.type = OPEN_BLOCK;
		}
		else if (open_block_char != EMPTY && characters.is_pairing (open_block_char, c)) {
			// This character closed the innermost open block
			token.type = CLOSE_BLOCK;
		}
		else if (characters.has_characteristic (c, CLOSE_BLOCK)) {
			error ("Unpaired closing character: " + c + " on line " + token.line_no, is);
			return rough_token (is, in_quoted_section, open_block_char);  // skip to next token
		}
		// Check for a sequence of spaces
		else if (characters.has_characteristic (c, SPACE)) {
			token.type = SPACE_SEQUENCE;
			while (characters.has_characteristic (peek_character (is), SPACE)) {
				c = next_character (is);
				token.text += c;
				if (characters.has_characteristic (c, NEW_LINE))
					token.new_line_count++;
			}
		}
		// Rough word token
		else {
			token.type = ROUGH_TOKEN;
			for (;;) {
				c = peek_character (is);  // what is the next character?
				if (c == EMPTY ||
					characters.has_characteristic (c, OPEN_BLOCK) ||
					characters.has_characteristic (c, CLOSE_BLOCK) ||
					characters.has_characteristic (c, SPACE))
					break;

				next_character (is); // consume character

				// Is character valid? (All characters are valid in quoted sections.)
				if (characters.is_valid (c) || in_quoted_section)
					token.text += c;
				else
					error ("Non-interpretable character outside of quote: " + c +
						"Byte sequence: " + characters.byte_string(c) + " at line " + token.line_no, is);
			}
		}

		return token;
	}

	void gen_rough_tokens (InputStat & is) {
		rough_tokens.clear ();

		// Initial outer token
		Token t (1, false);
		t.text = EMPTY;
		t.type = START_OF_INPUT;
		rough_tokens.push_back (t);

		gen_rough_tokens_rec (is, false, & rough_tokens.back());
	}

	void gen_rough_tokens_rec (InputStat & is, bool in_quoted_section, Token * outer) {
		for (;;) {
			Token t = rough_token (is, in_quoted_section, outer->text);
			if (t.type == END_OF_INPUT) {
				if (outer->text != EMPTY) {
					// Some block has not been closed properly.
					error ("Unpaired opening character: " + outer->text +
						" on line " + outer->line_no, is);
					// Repair.
					Token repair (is.line_no, in_quoted_section);
					repair.text = characters.get_default_pairing (outer->text);
					repair.type = CLOSE_BLOCK;
					repair.outer = outer;
					rough_tokens.push_back (repair);
				}
				return;
			}

			// What kind of token is this?
			t.outer = outer;
			rough_tokens.push_back (t);
			if (t.type == OPEN_BLOCK) {
				bool in_quote = (in_quoted_section ||  // either: in a quote or starting a quote?
					characters.has_characteristic (t.text, QUOTE));
				gen_rough_tokens_rec (is, in_quote, & rough_tokens.back());
			}
			else if (t.type == CLOSE_BLOCK) {
				return;
			}
		}
	}

	String print_rough_tokens () {
		StringBuffer b;
		std::vector<Token>::const_iterator i;
		for (i = rough_tokens.begin(); i != rough_tokens.end(); i++) {
			b += i->text + " ";
		}
		return b;
	}

	std::vector<Token>::const_iterator rough_token_iterator;
	String active_language;

	const Token * get_next_rough () {
		const Token * rough = peek_rough ();
		if (rough != NULL)
			rough_token_iterator++;
		return rough;
	}

	const Token * peek_rough (size_t fwd = 1) const {
		// Peek some number of tokens ahead.
		std::vector<Token>::const_iterator i = rough_token_iterator;
		for (; fwd > 0; fwd--) {
			if (i == rough_tokens.end())  // End of input?
				return NULL;
			i++;
		}
		return & (*rough_token_iterator);
	}

	void gen_clean_tokens (InputStat & is) {
		// Scan rough_tokens to generate clean (interpreted) tokens.
		rough_token_iterator = rough_tokens.begin ();
		active_language = EMPTY;
		clean_tokens.clear ();
		gen_clean_tokens_rec (is);
	}

	bool equals (const Token * t, const String & s) {
		return t && t->text == s;
	}

	bool is_one_of (const Token * t, const String choices[], size_t choice_count) {
		// Returns true if string s is in choices array
		if (t) {
			size_t i;
			for (i = 0; i < choice_count; i++)
				if (t->text == choices[i])
					return true;
		}
		return false;
	}

	bool is_space (const Token * t) {
		// Returns true if token t is a simple space sequence (not including newlines)
		if (t == NULL)
			return false;
		if (t->type != SPACE_SEQUENCE)
			return false;
		if (t->new_line_count > 0)
			return false;
		return true;
	}

	void gen_clean_tokens_rec (InputStat & is) {
		for (;;) {
			const Token * rough = get_next_rough ();

			// End of input?
			if (rough == NULL)
				return;
			// End of this block?
			else if (rough->type == CLOSE_BLOCK) {
				clean_tokens.push_back (*rough);
				active_language = EMPTY;
				return;
			}
			// Start of a new block?
			else if (rough->type == OPEN_BLOCK) {
				// Reduction 1: simple annotations
				if (rough->text == "[") {
					const size_t language_count = 5;
					String known_languages[language_count] = { "lit", "lat", "heb", "grk", "kat" };
					size_t fwd = 1;
					if (is_space (peek_rough (fwd))) // skip spaces
						fwd++;
					const Token * l = peek_rough (fwd);
					fwd++;
					if (is_one_of (l, known_languages, language_count)) {
						if (is_space (peek_rough (fwd))) // skip spaces
							fwd++;
						if (equals (peek_rough (fwd), "]")) {
							// Matched a language annotation.
							active_language = l->text;
							continue;
						}
					}
				}
				clean_tokens.push_back (*rough);
				gen_clean_tokens_rec (is);
			}
			// Rough token reduction
			else if (rough->type == ROUGH_TOKEN) {
			}
		}
	}

	void error (String error_text, InputStat & is) {
		is.errors.push_back (error_text + " in file " + is.name);
	}

	void section_interpreted () {
	}

	void section_quoted () {
	}
};

void Symbols::parse (const String & name, const String & input) {
	Parser::InputStat is (name, input);
	Parser parser;
	parser.gen_rough_tokens (is);
	parser.gen_clean_tokens (is);
	// retrieve tree
}

void Symbols::WriteCharacterReferenceHtml (std::ostream & output) {
	characters.WriteReferenceHtml (output);
}

