#include "algorithms.h"
#include "parser.h"
#include "instructions.h"
#include <string>
#include <sstream>
#include <cassert>
#include <vector>
#include <iostream>
#include <stdexcept>

#ifndef CODE_SEGMENT_OFFSET
static constexpr uint32_t CODE_SEGMENT_OFFSET = 0x00400000;
#endif

namespace mips {

Parser::Parser(std::ifstream &file) {
    CollectLabelsAndFunctions(file);
    CollectInstructions(file);
}

void Parser::ParseRTypeInstruction(uint32_t opcode, std::vector<std::string> &&tokens, uint32_t line_number) {
    if (tokens.size() != 4) {
        throw UnexpectedSymbolException((tokens.size() > 0 ? tokens[tokens.size() - 1] : ""), line_number);
    }
    if (IsRegister(tokens[1])) {
        if (IsRegister(tokens[2])) {
            if (IsRegister(tokens[3])) {
                instructions_.emplace_back(opcode, std::move(tokens));
            } else {
                throw RegisterNameExpectedException(tokens[2], line_number);
            }
        } else {
            throw RegisterNameExpectedException(tokens[2], line_number);
        }
    } else {
        throw RegisterNameExpectedException(tokens[1], line_number);
    }
}

void Parser::ParseImmediateInstruction(uint32_t opcode, std::vector<std::string> &&tokens, uint32_t line_number) {
    if (tokens.size() != 4) {
        throw UnexpectedSymbolException((tokens.size() > 0 ? tokens[tokens.size() - 1] : ""), line_number);
    }
    if (IsRegister(tokens[1])) {
        if (IsRegister(tokens[2])) {
            if (IsImmediateValue(tokens[3])) {
                instructions_.emplace_back(opcode, std::move(tokens));
            } else {
                throw UnexpectedSymbolException(tokens[2], line_number,
                                                "Expected immediate value.");
            }
        } else {
            throw RegisterNameExpectedException(tokens[2], line_number);
        }
    } else {
        throw RegisterNameExpectedException(tokens[1], line_number);
    }
}

void Parser::ParseBranchInstruction(uint32_t opcode, std::vector<std::string> &&tokens,
                                    uint32_t line_number, uint32_t instruction_number) {
    if (tokens.size() != 4) {
        throw UnexpectedSymbolException((tokens.size() > 0 ? tokens[tokens.size() - 1] : ""), line_number);
    }
    if (IsRegister(tokens[1])) {
        if (IsRegister(tokens[2])) {
            if (IsImmediateValue(tokens[3])) {
                instructions_.emplace_back(opcode, std::move(tokens));
            } else {
                auto found = labels_.find(tokens[3]);
                if (found != labels_.end()) {
                    auto const &pair = *found;
                    tokens[3] = std::to_string(static_cast<int32_t>(pair.second.first)
                                               - static_cast<int32_t>(instruction_number) - 2);
                    instructions_.emplace_back(opcode, std::move(tokens));
                } else {
                    throw UnexpectedSymbolException(tokens[2], line_number,
                                                    "Expected immediate value or label name.");
                }
            }
        } else {
            throw RegisterNameExpectedException(tokens[2], line_number);
        }
    } else {
        throw RegisterNameExpectedException(tokens[1], line_number);
    }
}

void Parser::ParseMemoryInstruction(uint32_t opcode, std::vector<std::string> &&tokens,
                                    uint32_t line_number)
{
    if (tokens.size() != 3) {
        throw UnexpectedSymbolException((tokens.size() > 0 ? tokens[tokens.size() - 1] : ""), line_number);
    }
    if (IsRegister(tokens[1])) {
        std::size_t open_paren_index = tokens[2].find_first_of('(');
        std::size_t close_paren_index = tokens[2].find_first_of(')');
        if (open_paren_index == std::string::npos) {
            throw UnexpectedSymbolException(tokens[2], line_number, "Expected \"(\".");
        }
        if (close_paren_index == std::string::npos) {
            throw UnexpectedSymbolException(tokens[2], line_number, "Expected \")\".");
        }
        std::string reg = tokens[2].substr(open_paren_index + 1, close_paren_index - open_paren_index - 1);
        if (!IsRegister(reg)) {
            throw RegisterNameExpectedException(tokens[2], line_number);
        }
        std::string value;
        if (open_paren_index == 0) {
            value = "0";
        } else {
            value = tokens[2].substr(0, open_paren_index);
        }
        tokens.pop_back();
        tokens.push_back(value);
        tokens.push_back(reg);
        instructions_.emplace_back(opcode, std::move(tokens));
    } else {
        throw RegisterNameExpectedException(tokens[1], line_number);
    }
}

void Parser::ParseJumpInstruction(uint32_t opcode, std::vector<std::string> &&tokens, uint32_t line_number)
{
    if (tokens.size() != 2) {
        throw UnexpectedSymbolException((tokens.size() > 0 ? tokens[tokens.size() - 1] : ""), line_number);
    }
    if (IsImmediateValue(tokens[1])) {
        instructions_.emplace_back(opcode, std::move(tokens));
    } else {
        auto value = labels_.find(tokens[1]);
        if (value != labels_.end()) {
            tokens[1] = std::to_string(static_cast<int32_t>(value->second.second));
            instructions_.emplace_back(opcode, std::move(tokens));
        } else {
            throw UnexpectedSymbolException(tokens[1], line_number,
                                            "Expected immediate value or label name.");
        }
    }
}

void Parser::ParseJALInstruction(uint32_t opcode, std::vector<std::string> &&tokens, uint32_t line_number)
{
    if (tokens.size() != 2) {
        throw UnexpectedSymbolException((tokens.size() > 0 ? tokens[tokens.size() - 1] : ""), line_number);
    }
    if (IsImmediateValue(tokens[1])) {
        instructions_.emplace_back(opcode, std::move(tokens));
    } else {
        auto value = functions_.find(tokens[1]);
        if (value != functions_.end()) {
            tokens[1] = std::to_string(value->second);
            instructions_.emplace_back(opcode, std::move(tokens));
        } else {
            throw UnexpectedSymbolException(tokens[1], line_number,
                                            "Expected immediate value or function name.");
        }
    }
}

void Parser::ParseJRInstruction(uint32_t opcode, std::vector<std::string> &&tokens, uint32_t line_number)
{
    if (tokens.size() != 2) {
        throw UnexpectedSymbolException((tokens.size() > 0 ? tokens[tokens.size() - 1] : ""), line_number);
    }
    if (IsRegister(tokens[1])) {
        instructions_.emplace_back(opcode, std::move(tokens));
    } else {
        throw RegisterNameExpectedException(tokens[1], line_number);
    }
}

void Parser::ProcessTokens(std::vector<std::string> &&tokens, uint32_t line_number, uint32_t instruction_number) {
	uint32_t opcode;
    if (IsInstruction(tokens[0], &opcode)) {
		switch(opcode) {
		case Instruction::RTYPE:
            if (tokens[0] == "jr") {
                ParseJRInstruction(opcode, std::move(tokens), line_number);
            } else {
                ParseRTypeInstruction(opcode, std::move(tokens), line_number);
            }
        break;
        case Instruction::ADDI:
		case Instruction::ORI:
		case Instruction::ANDI:
        case Instruction::SLTI:
            ParseImmediateInstruction(opcode, std::move(tokens), instruction_number);
			break;
        case Instruction::BEQ:
        case Instruction::BNE:
            ParseBranchInstruction(opcode, std::move(tokens), line_number, instruction_number);
            break;
		case Instruction::LW:
		case Instruction::SW:
            ParseMemoryInstruction(opcode, std::move(tokens), line_number);
			break;
        case Instruction::J:
            ParseJumpInstruction(opcode, std::move(tokens), line_number);
            break;
        case Instruction::JAL:
            ParseJALInstruction(opcode, std::move(tokens), line_number);
            break;
        default:
            throw UnexpectedSymbolException(tokens[0], line_number,
                                            "Invalid opcode for instruction.");
		}
	} else {
        throw UnexpectedSymbolException(tokens[0], line_number, "Invalid instruction.");
	}
}

bool Parser::IsInstruction(std::string const &value, uint32_t *opcode) {
	assert(opcode != nullptr);
    static constexpr char instruction_strings[][5] = {"add", "sub", "slt", "or", "and", "beq",
                                                      "bne", "addi", "ori", "andi", "slti", "sw",
                                                      "lw", "jal", "j", "jr"};

    static constexpr Instruction::Opcode opcodes[] = {Instruction::RTYPE, Instruction::RTYPE, Instruction::RTYPE,
                                                      Instruction::RTYPE, Instruction::RTYPE, Instruction::BEQ,
                                                      Instruction::BNE, Instruction::ADDI, Instruction::ORI,
                                                      Instruction::ANDI, Instruction::SLTI, Instruction::SW,
                                                      Instruction::LW, Instruction::JAL, Instruction::J,
                                                      Instruction::RTYPE };

    static constexpr size_t n_instructions = sizeof(opcodes) / sizeof(opcodes[0]);

    static_assert(sizeof(instruction_strings) / sizeof(instruction_strings[0]) == n_instructions,
            "Number of instruction strings doesn't match number of instruction opcodes.");

    auto ival = std::find(std::begin(instruction_strings), std::end(instruction_strings), value);
    if (ival != std::end(instruction_strings)) {
        *opcode = opcodes[ival - std::begin(instruction_strings)];
        return true;
    }
    return false;
}

bool Parser::IsRegister(std::string const &value) {
    if (value.length() == 3) {
        if (value[0] == '$') {
            if (value[1] == 't') {
                if (value[2] <= '7' && value[2] >= '0') {
					return true;
				}
            } else if (value[1] == 'a') {
                if (value[2] >= '0' && value[2] <= '1') {
					return true;
				}
            } else if (value[1] == 'v') {
                if (value[2] >= '0' && value[2] <= '1') {
                    return true;
                }
            } else if (value[1] == 's') {
                if ((value[2] >= '0' && value[2] <= '7') || value[2] == 'p') {
					return true;
				}
            } else if (value[1] == 'r' && value[2] == 'a') {
                return true;
            }
		}
	} else if (value == "$zero") {
		return true;
	}
	return false;
}

bool Parser::IsImmediateValue(std::string const &value) {
    bool hex = value.length() > 2 && value[0] == '0' && value[1] == 'x';
    if (hex) {
        if (pp::contains_which_not(std::begin(value) + 2, std::end(value),
                                  [](char c) {
                                      return (c >= '0' && c <= '9') && !(c >= 'a' && c <= 'f');
                                  })) {
            return false;
        }
	} else {
        int offset = (value[0] == '-');
        if (pp::contains_which(std::begin(value) + offset, std::end(value),
                              [](char c) { return c < '0' || c > '9'; })) {
            return false;
		}
	}
	return true;
}

void Parser::CollectLabelsAndFunctions(std::ifstream &file) {
    uint32_t instruction_number = 0;
    uint32_t line_number = 1;
    std::vector<std::pair<std::string, uint32_t>> labels_before_function;

    std::string line;
    while(std::getline(file, line)) {
        auto colon_it = std::find(std::begin(line), std::end(line), ':');
        if (colon_it != std::end(line)) {
            if (pp::contains_which_not(colon_it + 1, std::end(line), isspace)) {
                throw UnexpectedSymbolException(line, line_number, "Unexpected symbol after label.");
            }
            auto label_start = std::find_if_not(std::begin(line), std::end(line), isspace);

            if (pp::contains_which(label_start, colon_it, [](char c) { return !(isalnum(c) || c == '_'); })) {
                    throw UnexpectedSymbolException(line, line_number,
                                                    "Label name can only contain alpha-numeric characters and underscores");
            }
            std::string label_name = line.substr(label_start - std::begin(line), colon_it - label_start);

            labels_before_function.emplace_back(label_name, instruction_number);
            labels_[label_name] = std::pair(instruction_number + 1, CODE_SEGMENT_OFFSET + instruction_number * 4);
        } else {
            static std::string const expr = ".end";
            auto dot_end = std::search(std::begin(line), std::end(line), std::begin(expr), std::end(expr));
            if (dot_end != std::end(line)) {
                if (dot_end != std::begin(line)) {
                    throw UnexpectedSymbolException(line, line_number);
                }

                auto end = std::find_if (std::rbegin(line), std::rend(line), isspace);

                std::string function_name = line.substr(5, line.length() - (end - std::rbegin(line) - 4));
                auto pair = std::find_if (labels_before_function.begin(), labels_before_function.end(),
                                         [&function_name](std::pair<std::string, uint32_t> const &value) {
                                             return value.first == function_name;
                                         });
                if (pair != labels_before_function.end()) {
                    functions_[function_name] = CODE_SEGMENT_OFFSET + pair->second * 4;
                    labels_before_function.clear();
                } else {
                    throw UnexpectedSymbolException(line, line_number,
                                                    "Expected name of previously defined label.");
                }
            } else {
                if (!(line.empty() || line[0] == '#')) {
                    ++instruction_number;
                }
            }
        }
        ++line_number;
    }
}

void Parser::CollectInstructions(std::ifstream &file) {
	std::string line;
    uint32_t instruction_number = 0;
    uint32_t line_number = 0;
    file.clear();
	file.seekg(0, std::ios::beg);
	while(std::getline(file, line)) {
        ++line_number;
        if (line.empty() || line[0] == '#'
           || line.find_first_of(':') != std::string::npos
           || line.find(".end") != std::string::npos) {
			continue;
		}

        auto it = std::find_if_not(std::begin(line), std::end(line), isspace);
        if (it != line.end() && *it == '#') continue;

        auto end = std::find(std::begin(line), std::end(line), '#');

        std::string const delimeters = "\t ,;";
        std::vector<std::string> tokens;
        pp::split(std::begin(line), end,
                  std::begin(delimeters), std::end(delimeters) - 1,
                  std::back_inserter(tokens));
        ProcessTokens(std::move(tokens), line_number, instruction_number);
        ++instruction_number;
	}
}

UnexpectedSymbolException::UnexpectedSymbolException(const std::string &symbol, uint32_t line, const std::string &info) {
    message_ = "Unexpected symbol: \"" + symbol + "\" on line "
            + std::to_string(line) + ((info.empty()) ? "" : "\n" + info);
}

const char *UnexpectedSymbolException::what() const noexcept {
    return message_.c_str();
}

} // namespace mips

