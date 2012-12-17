/**
 A string class to hold unicode (UTF-8) and binary strings.
 Special functionality included to help the parser.
*/

#ifndef SPECIAL_STRING_H
#define SPECIAL_STRING_H

#include <cstring>
#include <iostream>
#include <list>
#include <sstream>
#include <string>
#include <vector>

class String : public std::vector<char> {
public:
	String () {
	}

	String (const String & string)
		: std::vector<char>(string.begin(), string.end()) {
	}

	String (const std::string & str)  // NULL-terminated C++ string
		: std::vector<char>(str.begin(), str.end()) {
	}

	String (char c) {
		push_back (c);
	}

	String (unsigned char c) {
		push_back ((signed) c);
	}

	String (int value) {
		std::ostringstream s;
		s << value;
		insert (begin(), s.str().begin(), s.str().end());
	}

	String (unsigned int value) {
		std::ostringstream s;
		s << value;
		insert (begin(), s.str().begin(), s.str().end());
	}

	String (const char * cstr)  // NULL-terminated C string
		: std::vector<char>(cstr, cstr + strlen(cstr)) {
	}

	String (const char * char_buf, size_t characters)  // C-style character buffer (can include NULL character)
		: std::vector<char>(char_buf, char_buf + characters) {
	}

	String (const String & first, const String & second) {  // addition of two strings
		reserve (first.size() + second.size());  // reserve space to prevent extra memory allocation
		insert (end(), first.begin(), first.end());
		insert (end(), second.begin(), second.end());
	}

	String (const char * first, const String & second) { // addition of two strings
		size_t first_size = strlen (first);
		reserve (first_size + second.size());  // reserve space to prevent extra memory allocation
		insert (end(), first, first + first_size);
		insert (end(), second.begin(), second.end());
	}

	String (const String & first, const char * second) { // addition of two strings
		size_t second_size = strlen (second);
		reserve (first.size() + second_size);  // reserve space to prevent extra memory allocation
		insert (end(), first.begin(), first.end());
		insert (end(), second, second + second_size);
	}

	const char * data() const { return &at(0); }

	operator std::string() const { return std::string (begin(), end()); }

	String slice (size_t from, size_t to) const {
		String s;
		if (to >= from) {
			s.reserve (to - from + 1);
			s.insert (s.begin(), begin() + from, begin() + to + 1);
		}
		return s;
	}

	void append (const String & string) {
		insert (end(), string.begin(), string.end());
	}
	void prepend (const String & string) {
		insert (begin(), string.begin(), string.end());
	}
	void operator += (const String & string) {
		insert (end(), string.begin(), string.end());
	}
	void operator += (const char * cstr) {
		insert (end(), cstr, cstr + strlen(cstr));
	}
	void operator += (char c) {
		push_back (c);
	}
	String operator + (const String & string) const {
		return String (*this, string);
	}
	String operator + (const char * cstr) const {
		return String (*this, cstr);
	}

	bool operator == (const char * cstr) const {
		size_t i;
		for (i = 0; cstr[i] != 0 && i < size(); i++) {
			if (cstr[i] != at(i))
				return false;
		}
		return i == size();
	}

	bool operator != (const char * cstr) const {
		size_t i;
		for (i = 0; cstr[i] != 0 && i < size(); i++) {
			if (cstr[i] != at(i))
				return true;
		}
		return i != size();
	}

	bool starts_with_exactly (const String & s) const {
		if (size() < s.size())
			return false;
		size_t i;
		for (i = 0; i < s.size(); i++) {
			if (at(i) != s[i])
				return false;
		}
		return true;
	}
};

/**
 StringBuffer keeps substrings separate until converted into a String.
 This allows many strings to be concatentated without too much unnecessary overhead.
 I wonder if Hans Boehm's rope implementation would add additional features.
 */
class StringBuffer {
	std::list<String> _parts;
public:
	StringBuffer () {}

	StringBuffer (const String & string) {
		_parts.push_back (string);
	}

	void append (const String & string) {
		_parts.push_back (string);
	}
	void append (const StringBuffer & buffer) {
		_parts.insert (_parts.begin(), buffer._parts.begin(), buffer._parts.end());
	}

	void prepend (const String & string) {
		_parts.push_front (string);
	}
	void prepend (const StringBuffer & buffer) {
		_parts.insert (_parts.begin(), buffer._parts.begin(), buffer._parts.end());
	}

	void operator += (const String & string) {
		_parts.push_back (string);
	}
	void operator += (const StringBuffer & buffer) {
		_parts.insert (_parts.end(), buffer._parts.begin(), buffer._parts.end());
	}

	operator String () const {
		String s;
		size_t size = 0;
		std::list<String>::const_iterator i;

		// Reserve enough bytes for concatenated string
		for (i = _parts.begin(); i != _parts.end(); i++)
			size += i->size();
		s.reserve (size);

		// Copy parts into string
		for (i = _parts.begin(); i != _parts.end(); i++)
			s += *i;
		return s;
	}
};

inline String operator + (const char * cstr, const String & string) {
	return String (cstr, string);
}

inline std::ostream& operator << (std::ostream & out, const String & string) {
	size_t i;
	for (i = 0; i < string.size(); i++)
		out << string[i];
	return out;
}

inline std::istream& operator >> (std::istream & in, String & string) {
	std::string s;
	in >> s;
	string = s;
	return in;
}

#endif
