#include "parser.h"
#include "mips.h"
#include <string>
#include <sstream>
#include <cassert>
#include <vector>
#include <iostream>
#include <stdexcept>

namespace mips {

Parser::Parser(std::ifstream &file) {
	std::string line;
	uint32_t line_number = 1;
	while(std::getline(file, line)) {
		if(line.empty() || line[0] == '#') {
			continue;
		}
		std::cout << "line: " << line << std::endl;
		std::size_t current;
		std::size_t previous = 0;
		current = line.find_first_of(" ,;");
		std::vector<std::string> tokens;
		while(current != std::string::npos) {
			tokens.push_back(line.substr(previous, current - previous));
			previous = current + 1;
			current = line.find_first_of(" ,;", previous);
		}
		tokens.push_back(line.substr(previous, current - previous));
		for(auto it = tokens.begin(); it != tokens.end();) {
			if(it->empty()) {
				tokens.erase(it);
			} else {
				++it;
			}
		}
		std::cout << "Tokens: ";
		for(auto &token : tokens) {
			std::cout << token << " ";
		}
		std::cout << std::endl;

		ProcessTokens(std::move(tokens), line_number);
		++line_number;
	}
	std::cout << "\nGathered instructions:\n";
	for(auto const &instruction : instructions_) {
		std::cout << "Opcode: " << std::hex << (instruction.opcode() >> 26u) << std::endl;
		std::cout << "Tokens: ";
		std::cout << "|";
		for(auto const &token : instruction.tokens()) {
			std::cout << token << "|";
		}
		std::cout << std::endl;
	}
}

void Parser::ProcessTokens(std::vector<std::string> &&tokens, uint32_t line) {
	uint32_t opcode;
	if(IsInstruction(tokens[0], &opcode)) {
		switch(opcode) {
		case Instruction::RTYPE:
			if(IsRegister(tokens[1])) {
				if(IsRegister(tokens[2])) {
					if(IsRegister(tokens[3])) {
						instructions_.emplace_back(opcode, std::move(tokens));
					} else {
						throw RegisterNameExpectedException(tokens[2], line);
					}
				} else {
					throw RegisterNameExpectedException(tokens[2], line);
				}
			} else {
				throw RegisterNameExpectedException(tokens[1], line);
			}
			break;
		case Instruction::ADDI:
		case Instruction::ORI:
		case Instruction::ANDI:
		case Instruction::BEQ:
        case Instruction::BNE:
            if(IsRegister(tokens[1])) {
				if(IsRegister(tokens[2])) {
					if(IsImmediateValue(tokens[3])) {
						instructions_.emplace_back(opcode, std::move(tokens));
					} else {
						throw UnexpectedSymbolException(tokens[2], line,
						                                "Expected immediate value.");
					}
				} else {
					throw RegisterNameExpectedException(tokens[2], line);
				}
			} else {
				throw RegisterNameExpectedException(tokens[1], line);
			}
			break;
		case Instruction::LW:
		case Instruction::SW:
			if(IsRegister(tokens[1])) {
				std::size_t open_paren_index = tokens[2].find_first_of('(');
				std::size_t close_paren_index = tokens[2].find_first_of(')');
				if(open_paren_index == std::string::npos) {
                    throw UnexpectedSymbolException(tokens[2], line, "Expected \"(\".");
                }
                if(close_paren_index == std::string::npos) {
                    throw UnexpectedSymbolException(tokens[2], line, "Expected \")\".");
				}
				std::string reg = tokens[2].substr(open_paren_index + 1, close_paren_index - open_paren_index - 1);
				if(!IsRegister(reg)) {
					throw RegisterNameExpectedException(tokens[2], line);
				}
				std::string value;
				if(open_paren_index == 0) {
					value = "0";
				} else {
					value = tokens[2].substr(0, open_paren_index);
				}
				tokens.pop_back();
				tokens.push_back(value);
				tokens.push_back(reg);
				instructions_.emplace_back(opcode, std::move(tokens));
			} else {
				throw RegisterNameExpectedException(tokens[1], line);
			}
			break;
		    default:
                throw UnexpectedSymbolException(tokens[0], line,
                                                "Invalid opcode for instruction.");
		}
	} else {
		throw UnexpectedSymbolException(tokens[0], line, "Invalid instruction.");
	}
}

bool Parser::IsInstruction(std::string const &value, uint32_t *opcode) {
	assert(opcode != nullptr);
	if(value == "add") {
		*opcode = Instruction::RTYPE;
		return true;
	} else if(value == "sub") {
		*opcode = Instruction::RTYPE;
		return true;
    } else if(value == "slt") {
        *opcode = Instruction::RTYPE;
        return true;
	} else if(value == "or") {
		*opcode = Instruction::RTYPE;
		return true;
	} else if(value == "and") {
		*opcode = Instruction::RTYPE;
		return true;
	} else if(value == "beq") {
		*opcode = Instruction::BEQ;
        return true;
    } else if(value == "bne") {
        *opcode = Instruction::BNE;
        return true;
	} else if(value == "addi") {
		*opcode = Instruction::ADDI;
		return true;
	} else if(value == "ori") {
		*opcode = Instruction::ORI;
		return true;
	} else if(value == "andi") {
		*opcode = Instruction::ANDI;
		return true;
	} else if(value == "sw") {
		*opcode = Instruction::SW;
		return true;
	} else if(value == "lw") {
		*opcode = Instruction::LW;
		return true;
	}
	return false;
}

bool Parser::IsRegister(std::string const &value) {
	if(value.length() == 3) {
		if(value[0] == '$') {
			if(value[1] == 't') {
				if(value[2] <= '7' && value[2] >= '0') {
					return true;
				}
			} else if(value[1] == 'a') {
				if(value[2] >= '0' && value[2] <= '1') {
					return true;
				}
			} else if(value[1] == 's') {
				if(value[2] >= '0' && value[2] <= '7') {
					return true;
				}
			}
		}
	} else if (value == "$zero") {
		return true;
	}
	return false;
}

bool Parser::IsImmediateValue(std::string const &value) {
	bool hex = false;
	if(value[0] == '0') {
		if(value[1] == 'x') {
			hex = true;
		}
	}
	if(hex) {
		for(auto it = value.begin() + 2; it != value.end(); ++it) {
			if(!(*it >= '0' && *it <= '9') && !(*it >= 'a' && *it <= 'f')) {
				return false;
			}
		}
	} else {
		std::string::const_iterator it;
		if(value[0] == '-') {
			it = value.begin() + 1;
		} else {
			it = value.begin();
		}
		for(; it != value.end(); ++it) {
			if(*it < '0' || *it > '9') {
				return false;
			}
		}
	}
	return true;
}

}
