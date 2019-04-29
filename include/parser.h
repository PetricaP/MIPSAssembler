#ifndef PARSER_H
#define PARSER_H

#include <fstream>
#include <vector>

namespace mips {

class UnexpectedSymbolException : public std::exception {
public:
	UnexpectedSymbolException(const std::string &symbol, uint32_t line) {
		message_ = "Unexpected symbol: \"" + symbol + "\" on line " + std::to_string(line) + ".";
	}

	const char *what() const noexcept override {
		return message_.c_str();
	}

private:
	std::string message_;
};

class Parser {
public:
	class InstructionData {
	public:
		InstructionData(uint32_t opcode = {}, std::vector<std::string> &&tokens = {})
			: opcode_(opcode), tokens_(tokens) {}

		uint32_t opcode() const { return opcode_; }
		std::vector<std::string> const &tokens() const { return tokens_; }
	private:
		uint32_t opcode_;
		std::vector<std::string> tokens_;
	};

	Parser(std::ifstream &file);

	std::vector<InstructionData> const &instructions() { return instructions_; }

private:
	bool IsInstruction(std::string const &value, uint32_t *opcode);
	bool IsRegister(std::string const &value);
	bool IsImmediateValue(std::string const &value);
	void ProcessTokens(std::vector<std::string> &&tokens, uint32_t line);

	std::vector<InstructionData> instructions_;
};

}

#endif // PARSER_H
