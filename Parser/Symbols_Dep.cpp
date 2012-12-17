#include "Symbols.h"

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

class Characters {
public:
	std::map<std::string,std::string> character_equivalence;
	std::multimap<std::string,std::string> inverse_character_equivalence;
	std::multimap<std::string,std::string> character_characteristics;
	std::multimap<std::string,std::string> inverse_character_characteristics;
	std::multimap<std::string,std::string> character_codes;
	std::map<std::string,std::string> inverse_character_codes;
	std::map<std::string,std::string> character_language;

	std::string active_language;
	std::set<std::string> active_characteristics;

	void set_equivalent (std::string equiv, std::string non_standard) {
		if (character_equivalence.count(non_standard) > 0) {
			breakpoint ();  // Error: Duplicate character
		}
		character_equivalence[non_standard] = equiv;
		inverse_character_equivalence.insert (std::make_pair (equiv, non_standard));
	}
	std::string get_equivalent (std::string non_standard) {
		if (character_equivalence.count(non_standard) > 0)
			return character_equivalence[non_standard];
		else
			return non_standard;
	}

	void add_code (std::string non_standard, std::string code) {
		// Adds a single code for a single character.
		character_codes.insert (std::make_pair (non_standard, code));
		if (inverse_character_codes.count(code) > 0) {
			breakpoint ();  // Error: duplicate character code
		}
		inverse_character_codes.insert (std::make_pair (code, non_standard));
	}
	void add_codes (std::string non_standard, std::string c1,
			std::string c2 = EMPTY, std::string c3 = EMPTY, std::string c4 = EMPTY) {
		// Adds a multiple codes for a single character.
		add_code (non_standard, c1);
		if (c2 != EMPTY) add_code (non_standard, c2);
		if (c3 != EMPTY) add_code (non_standard, c3);
		if (c4 != EMPTY) add_code (non_standard, c4);
	}
	std::set<std::string> get_codes (std::string non_standard) {
		// Retrieves character codes for a given character.
		std::set<std::string> codes;
		std::multimap<std::string,std::string>::iterator i;
		for (i = character_codes.lower_bound(non_standard);
			 i != character_codes.upper_bound(non_standard); i++) {
			codes.insert (i->second);
		}
		return codes;
	}
	std::string get_codes_string (std::string non_standard, std::string delimiter) {
		// Returns codes as a string delimited by delimiter.
		std::set<std::string> codes = get_codes (non_standard);
		std::string result = EMPTY;
		std::set<std::string>::iterator i;
		for (i = codes.begin(); i != codes.end(); i++) {
			result += *i + delimiter;
		}
		return result;
	}

	void add_characteristic (std::string non_standard, std::string characteristic) {
		// Adds a single characteristic to a single character.
		character_characteristics.insert (std::make_pair (non_standard, characteristic));
		inverse_character_characteristics.insert (std::make_pair (characteristic, non_standard));
	}
	void add_characteristics (std::string non_standard, std::string c1,
			std::string c2 = EMPTY, std::string c3 = EMPTY, std::string c4 = EMPTY) {
		// Adds a multiple characteristics to a single character.
		add_characteristic (non_standard, c1);
		if (c2 != EMPTY) add_characteristic (non_standard, c2);
		if (c3 != EMPTY) add_characteristic (non_standard, c3);
		if (c4 != EMPTY) add_characteristic (non_standard, c4);
	}
	void set_characteristics (std::string c1, std::string c2 = EMPTY,
			std::string c3 = EMPTY, std::string c4 = EMPTY, std::string c5 = EMPTY) {
		// Sets default characteristics for subsequent character additions.
		active_characteristics.clear ();
		if (c1 != EMPTY) active_characteristics.insert (c1);
		if (c2 != EMPTY) active_characteristics.insert (c2);
		if (c3 != EMPTY) active_characteristics.insert (c3);
		if (c4 != EMPTY) active_characteristics.insert (c4);
		if (c5 != EMPTY) active_characteristics.insert (c5);
	}
	std::set<std::string> get_characteristics (std::string non_standard) {
		// Retrieves character characteristics.
		// Characteristics are merged with equivalent character characteristics.
		std::set<std::string> characteristics;
		std::multimap<std::string,std::string>::iterator i;
		for (i = character_characteristics.lower_bound(non_standard);
			 i != character_characteristics.upper_bound(non_standard); i++) {
			characteristics.insert (i->second);
		}
		// Merge equivalent character characteristics.
		std::string equiv = get_equivalent (non_standard);
		for (i = character_characteristics.lower_bound(equiv);
			 i != character_characteristics.upper_bound(equiv); i++) {
			characteristics.insert (i->second);
		}
		return characteristics;
	}
	std::string get_characteristics_string (
			std::string non_standard, std::string delimiter) {
		// Returns characteristics as a string delimited by delimiter.
		std::set<std::string> characteristics = get_characteristics (non_standard);
		std::string result = EMPTY;
		std::set<std::string>::iterator i;
		for (i = characteristics.begin(); i != characteristics.end(); i++) {
			result += *i + delimiter;
		}
		return result;
	}

	void set_language (std::string non_standard, std::string language) {
		// Sets the language of a single character.
		character_language[non_standard] = language;
	}
	void set_language (std::string language) {
		// Sets default language for subsequent character additions
		active_language = language;
	}
	std::string get_language (std::string non_standard) {
		if (character_language.count(non_standard) > 0)
			return character_language[non_standard];
		else
			return EMPTY;
	}

	std::set<std::string> get_possible_languages (std::string non_standard) {
		std::set<std::string> languages;
		// If the character has no equivalent, add its language to the set.
		if (character_equivalence.count(non_standard) == 0) {
			if (character_language.count(non_standard) > 0)
				languages.insert (character_language[non_standard]);
		} else {
			// Else, search all equivalent characters, adding their languages to the set.
			std::string equiv = character_equivalence[non_standard];
			std::multimap<std::string,std::string>::iterator i;
			for (i = inverse_character_equivalence.lower_bound(equiv);
				 i != inverse_character_equivalence.upper_bound(equiv); i++) {
				if (character_language.count(i->second) > 0)
					languages.insert (character_language[i->second]);
			}
		}
		return languages;
	}

	void add_character () {
	}

	// Character classifications.
	//enum Classification {
		// Pointings normally modify the preceeding letter, so are considered alphabetic.
		// Spaces separate tokens.
		// Punctuation may separate tokens, but does not have to.
		// Consecutive letters/pointings/numbers must be in the same token.
		//LETTER, POINTING, NUMBER, STRUCTURAL, QUOTE,
		//SYMBOL, SPACE, FORMAT_CODE, PUNCTUATION, SPECIAL
		//LETTER = 0x01,
		//MARKING = 0x02,
		//DIGIT = 0x04,
		//OPEN_QUOTE = 0x10,
		//CLOSE_QUOTE = 0x20
		//LANGUAGE_SPECIFIC,  // this character is associated with a specific language
		//TOKEN,  // this character may be part of a symbolic token
		//SEPARATOR,  // this character separates tokens
		//EQUIVALENCY, // this character is mapped to an equivalent character or string
		//SPECIAL,  // this character may have a special lexical meaning

		//LETTER,   // this character is a letter (has a language associated)
		//MARK,     // this character is a combining mark or vowel pointing
		//DIGIT,    // this character is a decimal digit
		//SYMBOL,   // this character is a mathematical or international or other symbol
		//OPEN_QUOTE,  // this character may begin a quoted section
		//CLOSE_QUOTE, // this character may end a quoted section
		//SUPERSCRIPT,   // this character is in superscript
		//SUBSCRIPT,     // this character is in subscript
		//EQUIVALENCY,   // this character is mapped to an equivalent character or string
		//DISALLOWED_IN_QUOTE,       // this character is not ordinarily permitted inside a quoted section
		//DISALLOWED_OUTSIDE_QUOTE,  // this character is not ordinarily permitted outside a quoted section
		//NO_CLASSIFICATION = 0
	//};
	//std::map<Classification,std::pair<std::string,std::string>> classification_descriptions;
	//void init_classification_descriptions () {
	//	classification_descriptions[LETTER] = std::make_pair (
	//		"LETTER", "This character is a letter associated with a specific human language");
	//	classification_descriptions[MARK] = std::make_pair (
	//		"MARK", "This character is a combining mark or vowel pointing, usually treated like a letter");
	//	classification_descriptions[DIGIT] = std::make_pair (
	//		"DIGIT", "This character is a decimal digit");
	//	classification_descriptions[] = std::make_pair (
	//		"", "This character ");
	//	classification_descriptions[] = std::make_pair (
	//		"", "This character ");
	//	classification_descriptions[] = std::make_pair (
	//		"", "This character ");
	//	classification_descriptions[] = std::make_pair (
	//		"", "This character ");
	//	classification_descriptions[] = std::make_pair (
	//		"", "This character ");
	//	classification_descriptions[EQUIVALENCY] = std::make_pair (
	//		"EQUIVALENCY", "This character is mapped to another equivalent character or string");
	//	classification_descriptions[] = std::make_pair (
	//		"", "This character ");
	//	classification_descriptions[] = std::make_pair (
	//		"", "This character ");
		//classification_descriptions[] = std::make_pair (
		//	"", "This character ");
	//}
	//std::string get_classifications (unsigned int c, std::string delimiter = "\n") {
	//	std::string result;
	//	std::map<Classification,std::pair<std::string,std::string>>::iterator i;
	//	for (i = classification_descriptions.begin(); i != classification_descriptions.end(); i++) {
	//		if ((i->first & c)) {  // if this classification is present, add its name to result
	//			result += i->second.first + delimiter;
	//		}
	//	}
	//	return result;
	//}

	/*
	std::map<Classification,std::pair<std::string,std::string>> classification_names;
	void init_classification_names () {
		classification_names[LETTER] = std::make_pair (
			"LETTER", "Sequences of letters form identifiers.");
		classification_names[POINTING] = std::make_pair (
			"POINTING", "Vowel pointing or other combining mark. Normally treated like a letter.");
		classification_names[NUMBER] = std::make_pair (
			"NUMBER", "Numeric digit. May or may not be part of a longer identifier, depending on context.");
		classification_names[STRUCTURAL] = std::make_pair (
			"STRUCTURAL", "Parens, brackets, etc.");
		classification_names[QUOTE] = std::make_pair (
			"QUOTE", "Quotation mark.");
		classification_names[SYMBOL] = std::make_pair (
			"CHARACTER SYMBOL", "Character symbols, including math symbols, that may or may not be part of a longer identifier, depending on context.");
		classification_names[SPACE] = std::make_pair (
			"SPACE", "Spaces, including tabs and line breaks, force separation of identifiers.");
		classification_names[FORMAT_CODE] = std::make_pair (
			"FORMAT CODE", "Format codes may affect visual presentation of code, but otherwise have no effect.");
		classification_names[PUNCTUATION] = std::make_pair (
			"PUNCTUATION", "Punctuation characters carry structural meaning, and are never part of identifiers.");
		classification_names[SPECIAL] = std::make_pair (
			"SPECIAL", "Characters with special lexical meaning.");
	}
	*/

	// Character indexes.
/*	std::multimap<std::string,std::string> ascii_to_utf8;  // possible utf-8 interpretations
	std::multimap<std::string,std::string> utf8_to_ascii;  // (inverse)
	std::map<std::string,std::string> normalized;  // utf-8 to look-alike (may be multi-character)
	std::map<std::string,std::string> language;    // utf-8 language

	std::multimap<std::string,std::string> structural_pairings;  // maps open structural to close structural
	*/

	//// Current language for adding characters.
	//std::string current_language;
	//std::set<std::string> language_codes;
	//void set_language (std::string lang) {
	//	current_language = lang;
	//	language_codes.insert (lang);
	//}

	//// Current classification for adding characters.
	//unsigned int current_classification;
	//void set_classification (unsigned int c) {
	//	current_classification = c;
	//}

	//// Character Indexes
	//std::map<std::string,unsigned int> classification;
	//std::multimap<std::string,std::string> code_to_utf8;
	//std::multimap<std::string,std::string> utf8_to_code;
	//std::map<std::string,std::string> language;             // letter characters have a language
	//std::map<std::string,std::string> equivalent;

	//void set_equivalent (std::string utf8, std::string equiv) {
	//	equivalent[utf8] = equiv;
	//}

	//void add_code (std::string utf8, std::string code) {
	//	utf8_to_code.insert (std::make_pair (utf8, code));
	//	code_to_utf8.insert (std::make_pair (code, utf8));
	//}

	//void add (std::string utf8, std::string code_1 = "", std::string code_2 = "", std::string code_3 = "") {
	//	classification[utf8] |= current_classification;
	//	language[utf8] = current_language;
	//	if (code_1 != "")
	//		add_code (utf8, code_1);
	//	if (code_2 != "")
	//		add_code (utf8, code_2);
	//	if (code_3 != "")
	//		add_code (utf8, code_3);
	//}

	//std::multimap<std::string,std::string> quotations_nestable;  // open-quote -> close-quote
	//void add_quote_pair (std::string first, std::string second,
	//	std::string code_first, std::string code_second,
	//	std::string code_first_2="", std::string code_second_2="")
	//{
	//	quotations_nestable.insert (std::make_pair (first, second));
	//	set_classification (OPEN_QUOTE);
	//	add (first, code_first, code_first_2);
	//	set_classification (CLOSE_QUOTE);
	//	add (second, code_second, code_second_2);
	//}
	//void init_quotation_marks () {
	//	set_language ("");
	//	add_quote_pair ("\xc2\xab", "\xc2\xbb", "<<", ">>", "\\laquo\\", "\\raquo\\");
	//	add_quote_pair ("\xe2\x80\x9c", "\xe2\x80\x9d", "\\``\\", "\\''\\", "\\ldquo\\", "\\rdquo\\");
	//	add_quote_pair ("\xe2\x80\x9e", "\xe2\x80\x9c", "\\,,\\", "\\``\\", "\\bdquo\\", "\\ldquo\\");
	//	add_quote_pair ("\xe2\x80\x9e", "\xe2\x80\x9d", "\\,,\\", "\\''\\", "\\bdquo\\", "\\rdquo\\");
	//	add_quote_pair ("\xe2\x80\x98", "\xe2\x80\x99", "\\`\\", "\\'\\", "\\lsquo\\", "\\rsquo\\");
	//	add_quote_pair ("\xe3\x80\x8c", "\xe3\x80\x8d", "\\quoteL\\", "\\quoteLstop\\");
	//	add_quote_pair ("\xe3\x80\x8e", "\xe3\x80\x8f", "\\whiteL\\", "\\whiteLstop\\");
	//	set_classification (IN_QUOTED);
	//	add ("\xe2\x80\x9a", "\\,\\", "\\sbquo\\");
	//	set_classification (OUTSIDE_QUOTED);
	//	add ("\"");
	//}

/*
	void add (std::string utf8, std::string norm, Classification cl,
		std::string ascii, std::string ascii2="", std::string ascii3="")
	{
		// Add ascii equivalents to index
		ascii_to_utf8.insert (std::make_pair (ascii, utf8));
		utf8_to_ascii.insert (std::make_pair (utf8, ascii));
		if (ascii2 != "") {
			ascii_to_utf8.insert (std::make_pair (ascii2, utf8));
			utf8_to_ascii.insert (std::make_pair (utf8, ascii2));
		}
		if (ascii3 != "") {
			ascii_to_utf8.insert (std::make_pair (ascii3, utf8));
			utf8_to_ascii.insert (std::make_pair (utf8, ascii3));
		}

		// Add normal form and classification to index
		normalized[utf8] = norm;
		classification[utf8] = cl;

		// Add language, if a letter or pointing.
		if (cl == LETTER || cl == POINTING)
			language[utf8] = current_language;
		else
			language[utf8] = "";  // No specific language for numbers, symbols, etc.
	}

	void add_structural_pair (std::string open, std::string close) {
		structural_pairings.insert (std::make_pair (open, close));
	}
	*/
	Characters ();

	std::string get_codes_html (std::string utf8) {
		std::string equivs = "";
		std::multimap<std::string,std::string>::iterator j;
		for (j = utf8_to_code.lower_bound (utf8); j != utf8_to_code.upper_bound (utf8); j++) {
			equivs += escape (j->second) + "<br/>";
		}
		return equivs;
	}

	void WriteReferenceHtml (std::ostream & output) {
		std::map<std::string,unsigned int>::iterator i;
		std::multimap<std::string,std::string>::iterator mi;
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
			"<p>\n"
			<< std::endl;

		output <<
			"<table>\n"
			"<tr><th>Basic Convention</th><th>Explanation</th></tr>\n"
			"<tr><td>(Group of Things)</td><td>e.g.: subexpression, parameter list</td></tr>\n"
			"<tr><td>{Different Kind of Grouping of Things}</td><td>e.g.: set, code block</td></tr>\n"
			"<tr><td>[Comment for the Machine]</td><td>or, Annotation. Sometimes the Machine needs help interpreting the meaning of something.</td></tr>\n"
			"<tr><td>-- Comment for Humans</td><td>Humans need help with interpreting meanings too. (Or just disabling sections of code.)</td></tr>\n"
			"<tr><td>--- Documentation for Humans ---</td><td>Reference Material!</td></tr>\n"
			"<tr><td>A/B</td><td>B below A</td></tr>\n"
			"<tr><td>A\\B</td><td>B above A</td></tr>\n"
			"<tr><td>A-B</td><td>A and B together constitute a single thing</td></tr>\n"
			"<tr><td>\\code\\</td><td>Special Character Code</td></tr>\n"
			"<tr><td>"
				LDQUO"Quoted Literal"RDQUO"<br />\n"
				"\xe2\x80\x9eQuoted Literal"RDQUO"<br />\n"
				"\xe2\x80\x9eQuoted Literal"LDQUO"<br />\n"
				"\xc2\xabQuoted Literal\xc2\xbb<br />\n"
				"\xe2\x80\x98Quoted Literal\xe2\x80\x99<br />\n"
				"\xe3\x80\x8cQuoted Literal\xe3\x80\x8d<br />\n"
				"\xe3\x80\x8eQuoted Literal\xe3\x80\x8f<br />\n"
				"</td><td>Literal Quotation, nestable marks</td></tr>\n"
			"<tr><td>\"Quoted Literal\"</td><td>Literal Quotation, legacy marks</td></tr>\n"
			"</table>\n"
			<< std::endl;

		// --- as a quotation
		// -- as a range indicator
		// - as a line joiner
		// - as minus

		// [-] as literal hyphen
		// [---] as hbar

		// [ ... ] as code within string literal
		// [-- ... --] as comment within string literal

		output <<
			"<h2>Quotation Marks</h2>\n"
			<< std::endl;

		output <<
			"<h2>Legacy Concerns</h2>\n"
			"The inability to enter or display non-ASCII characters should never prevent "
			"programmers from creating, reading, or editing programs that use these characters.\n"
			"Editing program source in raw ASCII is such a common practice that "
			"it should receive all necessary support. It is unlikely that the practice of "
			"editing raw ASCII text files will disappear in the near future.\n"

			"<h3>Digraphs and Trigraphs</h3>\n"
			"With two exceptions, we assume that all non-control ASCII characters "
			"can be entered directly by the programmer. "
			"The exceptions are { and }, which may be represented by (* and *).\n"
			"According to the "LDQUO"Digraphs and trigraphs"RDQUO" entry on <i>Wikipedia</i>, "
			"these digraphs, supported by the Pascal language, were the only two "
			"programming language digraphs in wide use.\n"
			"(* and *) are only equivalent to { and } if correctly paired. "
			"Otherwise, the parser assumes the * is (or belongs to) another token.\n"
			<< std::endl;

		output <<
			"<h2>UTF-8</h2>\n"
			"Input text is assumed to be UTF-8, in a (more or less) canonical form. "
			"UTF-8 is widely used, well-supported, and backward-compatible with ASCII, "
			"so there does not appear (at this time) "
			"to be any reason to natively support any other character encoding.\n"

			"<h3>Look-Alike Characters</h3>\n"
			"Characters that are visually indistinguishable (or nearly so) "
			"are treated as identical.\n"
			"For example, the Latin letter A and the Greek letter \xce\x91 "
			"are fully interchangable without affecting the meaning of the program.\n"

			"<h3></h3>"

			"<h3>ASCII Character Sequences</h3>\n"
			"One design goal is to enable entry of any Unicode text "
			"with any keyboard that supports entry of ASCII text.\n"
			"Wherever reasonable, this entry should be possible without "
			"resorting to numeric escape codes (e.g., \\uxxxx or &amp;xxxx;).\n"
			"Numeric codes provide no visual information about the characters "
			"they represent, making text containing such codes exceedingly difficult "
			"to read and to write.\n"
			"<p>\n"
			"It is important to distinguish between interpretable characters and "
			"non-interpretable characters.\n"
			"The former may contribute to the meaning of the program, and the "
			"latter may only appear in contexts where the computer is never expected "
			"to assign any meaning, such as within quoted text.\n"
			"<p>\n"
			"Every interpretable character should be equivalent to some ASCII text string "
			"that describes or resembles that character in some way.\n"
			"For example, A\\` represents \xc3\x80, and PI represents \xce\xa0.\n"
			"Within non-interpretable text, the use of numeric codes is not always "
			"avoidable. The UTF-8 character set is very large and continually extended, "
			"and only a relatively small subset of these characters can (or should) "
			"be assigned ASCII sequences, but the lack of an easy-to-read ASCII sequence "
			"should not prevent programmers from using these characters in text string literals.\n"
			"<p>\n"
			"Some rules are necessary to disambiguate ASCII character sequences.\n"
			"These rules are described in the Tokens section.\n"
			<< std::endl;

		output <<
			"<h2>Tokens</h2>\n"
			"A token is a contiguous sequence of non-breaking characters.\n"
			"Breaking characters include spaces, quotation marks, structural characters, and punctuation. "
			"Other characters are considered non-breaking.\n"
			"Notably, the hyphen (-) is a token joiner.\n"
			"The hyphen is non-breaking, but is not itself included in the token.\n"
			"For example, the tokens <i>alphabet</i>, <i>alpha-bet</i>, and <i>a-l-p-h-a-b-e-t</i> are equivalent.\n"
			"The interpretation of hyphens is treated in greater detail in the Hyphens section.\n"
			"<p>\n"
			"Character sequences are matched greedily, under the provision that "
			"all letter characters in the token are of the same language.\n"
			"Multiple characters may need to be considered before a determination of the "
			"language is made.\n"
			"For example, the token <i>alphabet</i> is equivalent to itself, "
			"<i>alphabeta</i> is equivalent to \xce\xb1\xce\xb2, and "
			"<i>alefbet</i> is equivalent to \xd7\x90\xd7\x91.\n"
			"<p>\n"
			"Language codes may be placed at the start of a token to force a particular interpretation.\n"
			"For example, the code for Latin is <i>lat</i> and Greek is <i>grk</i>.\n"
			"The special language code <i>lit</i> forces a literal ASCII interpretation "
			"where no multi-character sequences are considered.\n"
			"For example, the tokens <i>IOTA\\..</i> and <i>grk-IOTA\\..</i> "
			"are equivalent to \xc3\x8f, <i>lat-IOTA\\..</i> "
			"is equivalent to IOT\xc3\x84, and <i>lit-IOTA\\..</i> "
			"yields IOTA\\.. unmodified.\n"
			"<p>\n"
			"Characters inside string literals are identified according to the same "
			"rules as characters outside string literals.\n"
			<< std::endl;

#if 0
		output <<
			"<h3>Tokens</h3>\n"
			"<p>\n"
			"Any contiguous sequence of letters, pointings, numbers, and character symbols "
			"is a primitive token.\n"
			<< std::endl;

		output <<
			"<h3>Binding Together: The Dash</h3>\n"
			"<p>\n"
			"Primitive tokens may be bound together into larger primitive tokens "
			"by connecting them with dashes.\n"
			"A maximal sequence of connected primitive tokens is a final token.\n"
			"A final token may be split into multiple final tokens between "
			"letters and numbers, letters and character symbols, and numbers and character symbols.\n"
			"This splitting occurs if a token is not defined, but "
			"some splitting yields a sequence of shorter tokens that are all defined.\n"
			"<p>\n"
			"Lines may be bound together by placing a dash as the last non-whitespace "
			"character before the end of a line.\n"
			"Tokens may not span multiple lines.\n"
			<< std::endl;

		output <<
			"<h3>Breaking Apart: The Space</h3>\n"
			<< std::endl;

		output <<
			"<h3>Multilingual Character Codes</h3>\n"
			"Some sequences of ASCII characters may be interpreted as Unicode characters.\n"
			"For example, A\\` is equivalent to \xc3\x80, alpha to \xce\xb1, "
			"and times to \xc3\x97.\n"
			"Character sequences are matched greedily.\n"
			"Character sequences yielding letters, or letter character sequences, "
			"have associated language codes. "
			"Normally, it is assumed that all letters in a token are of the "
			"same language. "
			"For example, "LDQUO"alphabeta"RDQUO" yields \xce\xb1\xce\xb2, "
			""LDQUO"alphabet"RDQUO" yields "LDQUO"alphabet"RDQUO", "
			"and "LDQUO"alefbet"RDQUO" yields "LDQUO"\xd7\x90\xd7\x91"RDQUO".\n"
			"The dash can be used to make divisions between letters more explicit. "
			"For example, "LDQUO"alpha-beta-gamma"RDQUO" and "LDQUO"alphabetagamma"RDQUO" "
			"are equivalent (but not "LDQUO"alphabet-agamma"RDQUO").\n"
			"The language code can be overridden by starting the token with a "
			"primitive token consisting of the desired language code. "
			"For example, Latin is "LDQUO"lat"RDQUO" and Greek is "LDQUO"grk"RDQUO".\n"
			"The language code "LDQUO"lit"RDQUO" yields a literal ASCII interpretation "
			"where no multi-character sequences are considered.\n"
			"For example, "LDQUO"IOTA\\.."RDQUO" and "LDQUO"grk-IOTA\\.."RDQUO" "
			"yield "LDQUO"\xc3\x8f"RDQUO", "LDQUO"lat-IOTA\\.."RDQUO" "
			"yields "LDQUO"IOT\xc3\x84"RDQUO", and "LDQUO"lit-IOTA\\.."RDQUO" "
			"yields "LDQUO"IOTA\\.."RDQUO".\n"
			/*
			"<p>\n"
			"Some letters have ambiguous language codes. "
			"For example, the Latin A and the Greek \xce\x86 look alike and are considered "
			"equivalent, but have different character codes. "
			"If this occurs, the first non-ambiguous letter is taken to be language of "
			"the entire token. "
			"<p>\n"
			"Character sequences are matched greedily. "
			"<p>\n"

			"The longest matching ASCII character sequence at the beginning of a token "
			"determines the language all character "
			"Character matching is greedy -- the longest sequence of ASCII characters "
			"that yields a single Unicode character is always chosen.\n"
			"Normally, the "*/
			<< std::endl;

		output <<
			"<h2>Character Literals</h2>\n"
			"<p>\n"
			"\n"
			"</p>\n"
			<< std::endl;

		output <<
			"<h2>Structural Formatting</h2>\n"
			"<table rules='rows'>\n"
			"<tr><th>Character</th><th>Pairing</th></tr>\n";

		for (mi = structural_pairings.begin(); mi != structural_pairings.end(); mi++) {
			if (classification[mi->first] == STRUCTURAL)
				output <<
					"<tr>" <<
					"<td>" << escape (mi->first) << "</td>" <<
					"<td>" << escape (mi->second) << "</td>" <<
					"</tr>\n";
		}
		output << "</table>\n" << std::endl;

		output <<
			"<h3>Structural Rules</h3>\n"
			"<p>\n"
			"Structural elements must be paired correctly, unless inside quotes."
			"</p>\n"
			<< std::endl;
#endif

		output <<
			"<h2>Quotes</h2>\n"
			"<table rules='rows'>\n"
			"<tr><th>Character</th><th>Pairing</th><th>Ascii 1</th><th>Ascii 2</th></tr>\n";
		for (mi = quotations_nestable.begin(); mi != quotations_nestable.end(); mi++) {
			output <<
				"<tr>" <<
				"<td>" << escape (mi->first) << "</td>" <<
				"<td>" << escape (mi->second) << "</td>" <<
				"<td>" << get_codes_html (mi->first) << "</td>" <<
				"<td>" << get_codes_html (mi->second) << "</td>" <<
				"</tr>\n";
		}
		output << "</table>\n" << std::endl;

		output <<
			"<h3>Handling of Quotes</h3>\n"
			"<p>\n"
			"To help prevent visual confusion between the single bottom quote (\xe2\x80\x9a) and the comma (,), "
			"outermost quotations may not begin with the single bottom quote.\n"
			"To help disambiguate the different uses of the ASCII single quote, "
			"outermost quotations may not begin with the ASCII single quote, "
			"and no attempt is made to match pairs of ASCII single quotes.\n"
			"Finnish and Swedish quotation styles (\xe2\x80\x9d"ELLIPSIS"\xe2\x80\x9d and \xe2\x80\x99"ELLIPSIS"\xe2\x80\x99 "
			"and \xc2\xbb"ELLIPSIS"\xc2\xbb) are not supported, nor are reversed quotation "
			"styles (\xc2\xbb"ELLIPSIS"\xc2\xab).\n"
			"However, such users may substitute ASCII double quote marks for outermost quotations, "
			"and prevent pairing of inner quote marks by using character "
			"literals. (E.g., \""ELLIPSIS"[\xe2\x80\x99]"ELLIPSIS"[\xe2\x80\x99]"ELLIPSIS"\".)\n"
			"French-style thin spaces next to quote marks (e.g., \xc2\xab\xe2\x80\xaf"ELLIPSIS"\xe2\x80\xaf\xc2\xbb\xe2\x80\xaf) "
			"are understood as literal spaces. Attempting to distinguish between literal spaces and "
			"spaces used for mere presentational formatting within strings may create too many ambiguities.\n"
			"Single-angle quote marks are not supported, although they may be embedded in quotations and comments."
			"</p>\n"
			<< std::endl;
		output <<
			"<p>\n"
			"Quotes, like other structural elements, may span multiple lines."
			"</p>\n"
			<< std::endl;

		output <<
			"<h2>Comments</h2>\n"
			"<p>\n"
			"Outside of a quote, a horizontal bar (\xe2\x80\x95) indicates the start of a comment.\n"
			"</p>\n"
			<< std::endl;

		output <<
			"<h2>Character List</h2>\n"
			"<table rules='rows'>\n"
			"<tr><th>Character</th><th>Ascii-Equivalent</th><th>Language</th></tr>\n";
		for (i = classification.begin(); i != classification.end(); i++) {
			output <<
				"<tr>" <<
				"<td>" << escape (i->first) << "</td>" <<
				"<td>" << get_codes_html (i->first) << "</td>" <<
				"<td>" << escape (language[i->first]) << "</td>" <<
				"</tr>\n";
		}
		output << "</table>\n" << std::endl;

		// FOOTER
		output <<
				"</body>\n"
				"</html>\n"
				<< std::endl;
	}

	std::string escape (std::string s) {
		size_t i;
		for (i = 0; i < s.size(); i++) {
			if (s[i] == '<')
				s.replace (i, 1, "&lt;");
			else if (s[i] == '>')
				s.replace (i, 1, "&gt;");
			else if (s[i] == '&')
				s.replace (i, 1, "&amp;");
		}
		return s;
	}
};

static Characters characters;

Characters::Characters () {
	init_classification_names ();

	// Ordinary Latin
	characters.set_language ("latin");

	characters.add ("!","!",SYMBOL,"!");
	characters.add ("\"","\"",QUOTE,"\"");  characters.add_structural_pair ("\"","\"");
	characters.add ("#","#",SYMBOL,"#");
	characters.add ("$","$",SYMBOL,"$");
	characters.add ("%","%",SYMBOL,"%");
	characters.add ("&","&",SYMBOL,"&");
	characters.add ("'","'",SYMBOL,"'");
	characters.add ("(","(",STRUCTURAL,"(");  characters.add_structural_pair ("(",")");
	characters.add (")",")",STRUCTURAL,")");
	characters.add ("*","*",SYMBOL,"*");
	characters.add ("+","+",SYMBOL,"+");
	characters.add (",",",",PUNCTUATION,",");
	characters.add ("-","-",SPECIAL,"-");
	characters.add (".",".",PUNCTUATION,".");
	characters.add ("/","/",SPECIAL,"/");

	characters.add ("0","0",NUMBER,"0");
	characters.add ("1","1",NUMBER,"1");
	characters.add ("2","2",NUMBER,"2");
	characters.add ("3","3",NUMBER,"3");
	characters.add ("4","4",NUMBER,"4");
	characters.add ("5","5",NUMBER,"5");
	characters.add ("6","6",NUMBER,"6");
	characters.add ("7","7",NUMBER,"7");
	characters.add ("8","8",NUMBER,"8");
	characters.add ("9","9",NUMBER,"9");

	characters.add (":",":",PUNCTUATION,":");
	characters.add (";",";",PUNCTUATION,";");
	characters.add ("<","<",SYMBOL,"<");
	characters.add ("=","=",SYMBOL,"=");
	characters.add (">",">",SYMBOL,">");
	characters.add ("?","?",SYMBOL,"?");
	characters.add ("@","@",SYMBOL,"@");
	characters.add ("[","[",STRUCTURAL,"[");  characters.add_structural_pair ("[","]");
	characters.add ("\\","\\",SPECIAL,"\\");
	characters.add ("]","]",STRUCTURAL,"]");
	characters.add ("^","^",SYMBOL,"^");
	characters.add ("_","_",SYMBOL,"_");
	characters.add ("`","`",SYMBOL,"`");
	characters.add ("{","{",STRUCTURAL,"{");  characters.add_structural_pair ("{","}");
	characters.add ("|","|",PUNCTUATION,"|");
	characters.add ("}","}",STRUCTURAL,"}");
	characters.add ("~","~",SYMBOL,"~");

	// Latin (supplemental)
	characters.set_language ("latin");

	characters.add ("\xc2\xa0","\xc2\xa0",SPACE,"\\nbsp\\");
	characters.add ("\xc2\xa6","|",PUNCTUATION,"\\brvbar\\");
	characters.add ("\xc2\xab","\xc2\xab",QUOTE,"<<");  add_structural_pair ("\xc2\xab","\xc2\xbb");
	characters.add ("\xc2\xb5","\xce\xbc",SYMBOL,"micro");
	characters.add ("\xc2\xb7",MIDDOT,PUNCTUATION,"\\middot\\");
	characters.add ("\xc2\xbb","\xc2\xbb",QUOTE,">>");

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
	characters.add ("\xd7\x83",":",PUNCTUATION,"\\sofpasuq\\");
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
	characters.add ("\xe3\x82\xa1","\xe3\x82\xa1",LETTER,"a");
	characters.add ("\xe3\x82\xa2","\xe3\x82\xa2",LETTER,"A");
	characters.add ("\xe3\x82\xa3","\xe3\x82\xa3",LETTER,"i");
	characters.add ("\xe3\x82\xa4","\xe3\x82\xa4",LETTER,"I");
	characters.add ("\xe3\x82\xa5","\xe3\x82\xa5",LETTER,"u");
	characters.add ("\xe3\x82\xa6","\xe3\x82\xa6",LETTER,"U");
	characters.add ("\xe3\x82\xa7","\xe3\x82\xa7",LETTER,"E");
	characters.add ("\xe3\x82\xa8","\xe3\x82\xa8",LETTER,"e");
	characters.add ("\xe3\x82\xa9","\xe3\x82\xa9",LETTER,"o");
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
}

//----------- !CHARACTERS

Symbols::Symbols () {
}

void Symbols::AddText (String name, const String & text) {
}

void Symbols::WriteCharacterReferenceHtml (std::ostream & output) {
	characters.WriteReferenceHtml (output);
}
