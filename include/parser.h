#ifndef PARSER_H
#define PARSER_H

#include <fstream>
#include <vector>
#include <unordered_map>

namespace mips {

class UnexpectedSymbolException : public std::exception {
public:
    UnexpectedSymbolException(std::string const &symbol, uint32_t line,
                              std::string const &info = {});

    const char *what() const noexcept;

private:
	std::string message_;
};

class RegisterNameExpectedException : public UnexpectedSymbolException {
public:
    RegisterNameExpectedException(std::string const &symbol, uint32_t line)
        : UnexpectedSymbolException(symbol, line, "Expected register name.") {}
};

class Parser {
public:
	class InstructionData {
	public:
		explicit InstructionData(uint32_t opcode = {}, std::vector<std::string> &&tokens = {})
			: opcode_(opcode), tokens_(tokens) {}

		uint32_t opcode() const { return opcode_; }
		std::vector<std::string> const &tokens() const { return tokens_; }
	private:
		uint32_t opcode_;
		std::vector<std::string> tokens_;
	};

	explicit Parser(std::ifstream &file);

	std::vector<InstructionData> const &instructions() { return instructions_; }

    void ParseImmediateInstruction();

    void ParseJALInstruction();

private:
	static bool IsInstruction(std::string const &value, uint32_t *opcode);
	static bool IsRegister(std::string const &value);
	static bool IsImmediateValue(std::string const &value);
    void ProcessTokens(std::vector<std::string> &&tokens, uint32_t line_number, uint32_t instruction_number);
	void CollectLabelsAndFunctions(std::ifstream &file);
    void CollectInstructions(std::ifstream &file);
    void ParseRTypeInstruction(uint32_t opcode, std::vector<std::string> &&tokens, uint32_t line_number);
    void ParseImmediateInstruction(uint32_t opcode, std::vector<std::string> &&tokens, uint32_t line_number);
    void ParseBranchInstruction(uint32_t opcode, std::vector<std::string> &&tokens, uint32_t line_number, uint32_t instruction_number);
    void ParseMemoryInstruction(uint32_t opcode, std::vector<std::string> &&tokens, uint32_t line_number);
    void ParseJumpInstruction(uint32_t opcode, std::vector<std::string> &&tokens, uint32_t line_number);
    void ParseJALInstruction(uint32_t opcode, std::vector<std::string> &&tokens, uint32_t line_number);
    void ParseJRInstruction(uint32_t opcode, std::vector<std::string> &&tokens, uint32_t line_number);

    std::vector<InstructionData> instructions_;
    std::unordered_map<std::string, std::pair<uint32_t, uint32_t>> labels_;
    std::unordered_map<std::string, uint32_t> functions_;
};

}

#endif // PARSER_H
