#ifndef UTIL_H
#define UTIL_H

// Appends a trailing '/' if not present.
inline std::string as_folder (std::string in) {
	char c = *(in.end() - 1);
	if (c == '/' || c == '\\')
		return in;
	else
		return in + '/';
}

#endif
