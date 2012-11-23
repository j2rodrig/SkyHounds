#include "libraries.h"

#include "errors.h"

static std::set<std::string> all_messages;

std::string Warning (std::string object_type, const char * file_name, int line,
		const std::string & message)
{
	// Check object type name. If NULL was passed to warning macro, object_type will be "int".
	if (object_type == "int")
		object_type = "";
	else
		object_type += " ";
	
	std::stringstream s;
	s << message << " [" << object_type << file_name << " " << line << "]\n";

	std::string full_message (s.str());
	
	// Output the message if it is not a duplicate.
	if (all_messages.count (full_message) == 0) {
		all_messages.insert (full_message);

		// Output to stderr
		fputs (full_message.c_str(), stderr);
		fflush (stderr);

		// Output to Visual Studio debug console
		OutputDebugString (full_message.c_str());
	}
	
	return full_message;
}

std::string Warning (std::string object_type, const char * file_name, int line,
		const char * format, ...)
{
	char message[1000];
	va_list arguments;
	va_start (arguments, format);
	vsnprintf (message, 1000, format, arguments);
	va_end (arguments);
	return Warning (object_type, file_name, line, std::string(message));
}
