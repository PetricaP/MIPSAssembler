#ifndef ASSEMBLER_H_
#define ASSEMBLER_H_

#include "instruction_factory.h"
#include <iostream>
#include <stdexcept>

namespace mips {

class FileNotFoundException : public std::exception {
public:
	FileNotFoundException(std::string const &file_path) {
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
	Assembler(std::string const &file_path)
		: file_path_(file_path) {
		std::ifstream file(file_path_);
		if(file.is_open()) {
			parser_ = std::make_unique<Parser>(file);
			auto const &data = parser_->instructions();
			for(auto instructionData : data) {
				instructions_.push_back(InstructionFactory::CreateInstruction(instructionData));
			}
			file.close();
		} else {
			throw FileNotFoundException(file_path);
		}
	}

	void WriteToFile(std::string const &file_path) {
		std::ofstream file(file_path);
		if(file.is_open()) {
			for(auto const &instruction : instructions_) {
				std::cout.fill('0');
				std::cout.width(8);
				std::cout << std::hex << instruction->GetInstruction();
				std::cout << std::endl;
				file.width(8);
				file.fill('0');
				file << std::right << std::hex << instruction->GetInstruction() << std::endl;
			}
			file.close();
		}
	}

private:
	std::string file_path_;
	std::unique_ptr<Parser> parser_;
	std::vector<std::unique_ptr<Instruction>> instructions_;
};

} // namespace mips

#endif // ASSEMBLER_H_
