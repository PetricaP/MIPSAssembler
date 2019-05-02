#ifndef ASSEMBLER_H_
#define ASSEMBLER_H_

#include "instruction_factory.h"
#include <iostream>
#include <stdexcept>
#include <bitset>

namespace mips {

class FileNotFoundException : public std::exception {
public:
	explicit FileNotFoundException(std::string const &file_path) {
		message_ = "File " + file_path + " was not found.";
	}

	const char *what() const noexcept override {
		return message_.c_str();
	}

private:
	std::string message_;
};

class Assembler {
public:
	explicit Assembler(std::string const &file_path);

	void WriteToFile(std::string const &file_path);

private:
	std::string file_path_;
	std::unique_ptr<Parser> parser_;
	std::vector<std::unique_ptr<Instruction>> instructions_;
};

} // namespace mips

#endif // ASSEMBLER_H_
