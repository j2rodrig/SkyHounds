#include "Symbols.h"

#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

// NOTES:
// The single-quote (') should never be used for character literals.
// It is overloaded for use in symbols (e.g., primes) and ascii character renderings.

//----------- CHARACTERS

#define MIDDOT   "\xc2\xb7"
#define ELLIPSIS   "\xe2\x80\xa6"


class Characters {
public:
	// Character classifications.
	enum Classification {
		// Pointings normally modify the preceeding letter, so are considered alphabetic.
		// Spaces separate tokens.
		// Punctuation may separate tokens, but does not have to.
		// Consecutive letters/pointings/numbers must be in the same token.
		LETTER, POINTING, NUMBER, STRUCTURAL, QUOTE,
		SYMBOL, SPACE, FORMAT_CODE, PUNCTUATION, SPECIAL
	};
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

	// Character indexes.
	std::multimap<std::string,std::string> ascii_to_utf8;  // possible utf-8 interpretations
	std::multimap<std::string,std::string> utf8_to_ascii;  // (inverse)
	std::map<std::string,std::string> normalized;  // utf-8 to look-alike (may be multi-character)
	std::map<std::string,std::string> language;    // utf-8 language
	std::map<std::string,Classification> classification;

	std::multimap<std::string,std::string> structural_pairings;  // maps open structural to close structural

	// Current language for adding characters.
	std::string current_language;
	void set_language (std::string lang) {
		current_language = lang;
	}

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

		// Add normal form, classification, and language to index
		normalized[utf8] = norm;
		language[utf8] = current_language;
		classification[utf8] = cl;
	}

	void add_structural_pair (std::string open, std::string close) {
		structural_pairings.insert (std::make_pair (open, close));
	}

	Characters ();

	std::string get_ascii_equivalents_html (std::string utf8) {
		std::string equivs = "";
		std::multimap<std::string,std::string>::iterator j;
		for (j = utf8_to_ascii.lower_bound (utf8); j != utf8_to_ascii.upper_bound (utf8); j++) {
			equivs += escape (j->second) + "<br/>";
		}
		return equivs;
	}

	void WriteReferenceHtml (std::ostream & output) {
		std::map<std::string,std::string>::iterator i;
		std::multimap<std::string,std::string>::iterator mi;
		// HEADER
		output <<
				"<!doctype html>\n"
				"<html>\n"
				"<head>\n"
				"<meta http-equiv='Content-Type' content='text/html; charset=UTF-8' />\n"
				"<style type='text/css'>\n"
				"th { padding-right: 4px; padding-left: 4px; text-align: center; }\n"
				"td { padding-right: 4px; padding-left: 4px; text-align: center; }\n"
				"</style>\n"
				""
				"<title>Character Set Reference</title>\n"
				""
				"</head>\n"
				"<body>\n";

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


		output <<
			"<h2>Quotes</h2>\n"
			"<table rules='rows'>\n"
			"<tr><th>Character</th><th>Pairing</th><th>Ascii 1</th><th>Ascii 2</th></tr>\n";

		for (mi = structural_pairings.begin(); mi != structural_pairings.end(); mi++) {
			if (classification[mi->first] == QUOTE)
				output <<
					"<tr>" <<
					"<td>" << escape (mi->first) << "</td>" <<
					"<td>" << escape (mi->second) << "</td>" <<
					"<td>" << get_ascii_equivalents_html (mi->first) << "</td>" <<
					"<td>" << get_ascii_equivalents_html (mi->second) << "</td>" <<
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
			"<tr><th>Character</th><th>Ascii-Equivalent</th><th>Utf8-Equivalent</th><th>Classification</th><th>Human Language</th></tr>\n";

		for (i = normalized.begin(); i != normalized.end(); i++) {

			output <<
				"<tr>" <<
				"<td>" << escape (i->first) << "</td>" <<
				"<td>" << get_ascii_equivalents_html (i->first) << "</td>" <<
				"<td>" << escape (i->second) << "</td>" <<
				"<td style='text-align:left'>" << escape (classification_names[classification[i->first]].first) << "</td>" <<
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
	characters.add ("\xc2\xb5","\xce\xbc",LETTER,"micro");
	characters.add ("\xc2\xb7",MIDDOT,SYMBOL,"\\middot\\");
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
	characters.add ("\xe2\x80\x8c"," ",SPACE,"\\zwnj\\");
	characters.add ("\xe2\x80\x8d","-",SPECIAL,"\\zwj\\");
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

	characters.add ("\xe2\x80\xa6","...",PUNCTUATION,"\\ellipsis\\","\\hellip\\");

	characters.add ("\xe2\x80\xaf","\xe2\x80\xaf",SPACE,"\\nnbsp\\");

	//characters.add ("","","","");
}

//----------- !CHARACTERS

void Symbols::WriteCharacterReferenceHtml (std::ostream & output) {
	characters.WriteReferenceHtml (output);
}
