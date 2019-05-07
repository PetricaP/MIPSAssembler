#include "parser.h"
#include "instructions.h"
#include <string>
#include <sstream>
#include <cassert>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <algorithm>

#ifndef CODE_SEGMENT_OFFSET
static constexpr uint32_t CODE_SEGMENT_OFFSET = 0x00400000;
#endif

namespace mips {

Parser::Parser(std::ifstream &file) {
    CollectLabelsAndFunctions(file);
    CollectInstructions(file);
}

void Parser::ParseRTypeInstruction(uint32_t opcode, std::vector<std::string> &&tokens, uint32_t instruction_number) {
    if(tokens.size() > 4) {
        if(tokens[4][0] != '#') {
            throw UnexpectedSymbolException(tokens[5], instruction_number);
        }
    }
    if(IsRegister(tokens[1])) {
        if(IsRegister(tokens[2])) {
            if(IsRegister(tokens[3])) {
                instructions_.emplace_back(opcode, std::move(tokens));
            } else {
                throw RegisterNameExpectedException(tokens[2], instruction_number);
            }
        } else {
            throw RegisterNameExpectedException(tokens[2], instruction_number);
        }
    } else {
        throw RegisterNameExpectedException(tokens[1], instruction_number);
    }
}

void Parser::ParseImmediateInstruction(uint32_t opcode, std::vector<std::string> &&tokens, uint32_t instruction_number) {
    if(tokens.size() > 4) {
        if(tokens[4][0] != '#') {
            throw UnexpectedSymbolException(tokens[4], instruction_number);
        }
    }
    if(IsRegister(tokens[1])) {
        if(IsRegister(tokens[2])) {
            if(IsImmediateValue(tokens[3])) {
                instructions_.emplace_back(opcode, std::move(tokens));
            } else {
                throw UnexpectedSymbolException(tokens[2], instruction_number,
                                                "Expected immediate value.");
            }
        } else {
            throw RegisterNameExpectedException(tokens[2], instruction_number);
        }
    } else {
        throw RegisterNameExpectedException(tokens[1], instruction_number);
    }
}

void Parser::ParseBranchInstruction(uint32_t opcode, std::vector<std::string> &&tokens,
                                    uint32_t instruction_number) {
    if(tokens.size() > 4) {
        if(tokens[4][0] != '#') {
            throw UnexpectedSymbolException(tokens[4], instruction_number);
        }
    }
    if(IsRegister(tokens[1])) {
        if(IsRegister(tokens[2])) {
            if (IsImmediateValue(tokens[3])) {
                instructions_.emplace_back(opcode, std::move(tokens));
            } else {
                auto found = labels_.find(tokens[3]);
                if(found != labels_.end()) {
                    auto const &pair = *found;
                    tokens[3] = std::to_string(static_cast<int32_t>(pair.second.first)
                                               - static_cast<int32_t>(instruction_number) - 2);
                    instructions_.emplace_back(opcode, std::move(tokens));
                } else {
                    throw UnexpectedSymbolException(tokens[2], instruction_number,
                                                    "Expected immediate value or label name.");
                }
            }
        } else {
            throw RegisterNameExpectedException(tokens[2], instruction_number);
        }
    } else {
        throw RegisterNameExpectedException(tokens[1], instruction_number);
    }
}

void Parser::ParseMemoryInstruction(uint32_t opcode, std::vector<std::string> &&tokens,
                                    uint32_t instruction_number)
{
    if(tokens.size() > 3) {
        if(tokens[3][0] != '#') {
            throw UnexpectedSymbolException(tokens[3], instruction_number);
        }
    }
    tokens.erase(tokens.begin() + 3, tokens.end());
    if(IsRegister(tokens[1])) {
        std::size_t open_paren_index = tokens[2].find_first_of('(');
        std::size_t close_paren_index = tokens[2].find_first_of(')');
        if(open_paren_index == std::string::npos) {
            throw UnexpectedSymbolException(tokens[2], instruction_number, "Expected \"(\".");
        }
        if(close_paren_index == std::string::npos) {
            throw UnexpectedSymbolException(tokens[2], instruction_number, "Expected \")\".");
        }
        std::string reg = tokens[2].substr(open_paren_index + 1, close_paren_index - open_paren_index - 1);
        if(!IsRegister(reg)) {
            throw RegisterNameExpectedException(tokens[2], instruction_number);
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
        throw RegisterNameExpectedException(tokens[1], instruction_number);
    }
}

void Parser::ParseJumpInstruction(uint32_t opcode, std::vector<std::string> &&tokens, uint32_t instruction_number)
{
    if(tokens.size() > 2) {
        if(tokens[2][0] != '#') {
            throw UnexpectedSymbolException(tokens[2], instruction_number);
        }
    }
    if(IsImmediateValue(tokens[1])) {
        instructions_.emplace_back(opcode, std::move(tokens));
    } else {
        auto value = labels_.find(tokens[1]);
        if(value != labels_.end()) {
            tokens[1] = std::to_string(static_cast<int32_t>(value->second.second));
            instructions_.emplace_back(opcode, std::move(tokens));
        } else {
            throw UnexpectedSymbolException(tokens[1], instruction_number,
                                            "Expected immediate value or label name.");
        }
    }
}

void Parser::ParseJALInstruction(uint32_t opcode, std::vector<std::string> &&tokens, uint32_t instruction_number)
{
    if(tokens.size() > 2) {
        if(tokens[2][0] != '#') {
            throw UnexpectedSymbolException(tokens[2], instruction_number);
        }
    }
    if(IsImmediateValue(tokens[1])) {
        instructions_.emplace_back(opcode, std::move(tokens));
    } else {
        auto value = functions_.find(tokens[1]);
        if(value != functions_.end()) {
            tokens[1] = std::to_string(value->second);
            instructions_.emplace_back(opcode, std::move(tokens));
        } else {
            throw UnexpectedSymbolException(tokens[1], instruction_number,
                                            "Expected immediate value or function name.");
        }
    }
}

void Parser::ParseJRInstruction(uint32_t opcode, std::vector<std::string> &&tokens, uint32_t instruction_number)
{
    if(tokens.size() > 2) {
        if(tokens[2][0] != '#') {
            throw UnexpectedSymbolException(tokens[2], instruction_number);
        }
    }
    if(IsRegister(tokens[1])) {
        instructions_.emplace_back(opcode, std::move(tokens));
    } else {
        throw RegisterNameExpectedException(tokens[1], instruction_number);
    }
}

void Parser::ProcessTokens(std::vector<std::string> &&tokens, uint32_t instruction_number) {
	uint32_t opcode;
	if(IsInstruction(tokens[0], &opcode)) {
		switch(opcode) {
		case Instruction::RTYPE:
            if(tokens[0] == "jr") {
                ParseJRInstruction(opcode, std::move(tokens), instruction_number);
            } else {
                ParseRTypeInstruction(opcode, std::move(tokens), instruction_number);
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
            ParseBranchInstruction(opcode, std::move(tokens), instruction_number);
            break;
		case Instruction::LW:
		case Instruction::SW:
            ParseMemoryInstruction(opcode, std::move(tokens), instruction_number);
			break;
        case Instruction::J:
            ParseJumpInstruction(opcode, std::move(tokens), instruction_number);
            break;
        case Instruction::JAL:
            ParseJALInstruction(opcode, std::move(tokens), instruction_number);
            break;
        default:
            throw UnexpectedSymbolException(tokens[0], instruction_number,
                                            "Invalid opcode for instruction.");
		}
	} else {
        throw UnexpectedSymbolException(tokens[0], instruction_number, "Invalid instruction.");
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

    for(size_t i = 0; i < n_instructions; ++i) {
        if(value == instruction_strings[i]) {
            *opcode = opcodes[i];
            return true;
        }
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
            } else if(value[1] == 'v') {
                if(value[2] >= '0' && value[2] <= '1') {
                    return true;
                }
            } else if(value[1] == 's') {
                if((value[2] >= '0' && value[2] <= '7') || value[2] == 'p') {
					return true;
				}
            } else if(value[1] == 'r' && value[2] == 'a') {
                return true;
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

void Parser::CollectLabelsAndFunctions(std::ifstream &file) {
    std::string line;
    uint32_t instruction_number = 0;

    std::vector<std::pair<std::string, uint32_t>> labels_before_function;
    size_t index;
    while(std::getline(file, line)) {
        index = line.find_first_of(':');
        if(index != std::string::npos) {
            for (size_t i = index + 1; i < line.length(); ++i) {
                if (!isspace(line[i])) {
                    throw UnexpectedSymbolException(line, instruction_number,
                                                    "Unexpected symbol after label.");
                }
            }
            size_t start_index = 0;
            while(isspace(line[start_index])) {
                ++start_index;
            }
            for (size_t i = start_index; i < index; ++i) {
                if (!(isalnum(line[i]) || line[i] == '_')) {
                    throw UnexpectedSymbolException(line, instruction_number,
                                                    "Label name can only contain alpha-numeric characters and underscores");
                }
            }
            std::string label_name = line.substr(start_index, index - start_index);
            labels_before_function.emplace_back(label_name, instruction_number);
            labels_[label_name] = std::pair(instruction_number + 1, CODE_SEGMENT_OFFSET + instruction_number * 4);
        } else {
            index = line.find(".end ");
            if(index != std::string::npos) {
                if(index != 0) {
                    throw UnexpectedSymbolException(line, instruction_number);
                }
                size_t end_index;
                for(end_index = line.length() - 1; end_index > 0 && isspace(line[end_index]); --end_index);

                std::string function_name = line.substr(5, end_index - 4);
                // label_name is the last label found
                auto pair = std::find_if(labels_before_function.begin(), labels_before_function.end(),
                                         [&function_name](std::pair<std::string, uint32_t> const &value) {
                                             return value.first == function_name;
                                         });
                if(pair != labels_before_function.end()) {
                    functions_[function_name] = CODE_SEGMENT_OFFSET + pair->second * 4;
                    labels_before_function.clear();
                } else {
                    throw UnexpectedSymbolException(line, instruction_number,
                                                    "Expected name of previously defined label.");
                }
            } else {
                if(line.empty() || line[0] == '#') {
                    continue;
                }
                ++instruction_number;
            }
        }
    }

}

void Parser::CollectInstructions(std::ifstream &file) {
	std::string line;
    uint32_t instruction_number = 0;
    file.clear();
	file.seekg(0, std::ios::beg);
	while(std::getline(file, line)) {
		if(line.empty() || line[0] == '#'
           || line.find_first_of(':') != std::string::npos
           || line.find(".end") != std::string::npos) {
			continue;
		}
        uint32_t index = 0;
        while(isspace(line[index])) {
            ++index;
        }
        if(line[index] == '#') continue;

        std::size_t current;
        std::size_t previous = 0;
        current = line.find_first_of("\t ,;");
        std::vector<std::string> tokens;
        while (current != std::string::npos) {
            tokens.push_back(line.substr(previous, current - previous));
            previous = current + 1;
            current = line.find_first_of("\t ,;", previous);
        }
        tokens.push_back(line.substr(previous, current - previous));
        for (auto it = tokens.begin(); it != tokens.end();) {
            if (it->empty()) {
                tokens.erase(it);
            } else {
                ++it;
            }
        }
        ProcessTokens(std::move(tokens), instruction_number);
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

