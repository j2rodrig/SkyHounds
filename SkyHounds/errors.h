#ifndef ERRORS_H
#define ERRORS_H

#define warning(object,...)  Warning(typeid(object).name(),__FILE__,__LINE__,__VA_ARGS__);

std::string Warning (std::string object_type, const char * file_name, int line,
		const std::string & message);

std::string Warning (std::string object_type, const char * file_name, int line,
		const char * format, ...);

#endif

