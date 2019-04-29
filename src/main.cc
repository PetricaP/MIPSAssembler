#include "assembler.h"
#include <iostream>
#include <cstring>

int main(int argc, char const *argv[]) {
	std::string src_file;
	std::string dest_file;
	if(argc == 1) {
		src_file = "test.s";
		dest_file = "code.mem";
	} else if(argc == 4 && strcmp(argv[2], "-o") == 0) {
		src_file = argv[1];
		src_file = argv[3];
	} else {
		std::cerr << "Invalid number of parameters " << argc << ".";
		std::cerr << "Usage: assembler <input_file> -o <output_file>\n";
		std::exit(EXIT_FAILURE);
	}
	try {
		mips::Assembler assembler("test.s");
		assembler.WriteToFile("code.mem");
	} catch(mips::FileNotFoundException &e) {
		std::cerr << "Error: ";
		std::cerr << e.what() << std::endl;
		exit(EXIT_FAILURE);
	} catch(mips::UnexpectedSymbolException &e) {
		std::cerr << "Error: ";
		std::cerr << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}
}
