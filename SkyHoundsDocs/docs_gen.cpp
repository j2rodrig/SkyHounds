#include "../Parser/Symbols.h"

#include <fstream>

void run_tests ();

int main () {
	std::ofstream char_ref ("character-reference.html");
	Symbols::WriteCharacterReferenceHtml (char_ref);

	run_tests();

	return 0;
}

void run_tests () {
	Symbols s;
	s.AddTextFromFile ("test1.txt", "test1.pre");
	s.AddTextFromFile ("test2.txt", "test2.pre");
}
