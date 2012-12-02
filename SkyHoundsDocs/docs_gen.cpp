#include <fstream>

#include "../Parser/Symbols.h"

int main () {
	std::ofstream char_ref ("character-reference.html");
	Symbols::WriteCharacterReferenceHtml (char_ref);
	return 0;
}
