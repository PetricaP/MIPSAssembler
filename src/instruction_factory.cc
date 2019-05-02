#include "instruction_factory.h"

namespace mips {

#define RETURN_RTYPE_INSTRUCTION(TYPE)\
			return std::make_unique<TYPE##Instruction>(\
						Instruction::RegisterNameToNumber(data.tokens()[1]),\
						Instruction::RegisterNameToNumber(data.tokens()[2]),\
						Instruction::RegisterNameToNumber(data.tokens()[3]),\
						0)

#define RETURN_IMMEDIATE_INSTRUCTION(TYPE)\
			return std::make_unique<TYPE##Instruction>(\
						Instruction::RegisterNameToNumber(data.tokens()[1]),\
						Instruction::RegisterNameToNumber(data.tokens()[2]),\
						static_cast<uint16_t>(std::strtoul(data.tokens()[3].c_str(), nullptr, 0)))

#define RETURN_MEMORY_INSTRUCTION(TYPE)\
			return std::make_unique<TYPE##Instruction>(\
						Instruction::RegisterNameToNumber(data.tokens()[1]),\
						static_cast<uint16_t>(std::strtoul(data.tokens()[2].c_str(), nullptr, 0)),\
						Instruction::RegisterNameToNumber(data.tokens()[3]))

std::unique_ptr<Instruction> InstructionFactory::CreateInstruction(
		const Parser::InstructionData &data) {
	switch(data.opcode()) {
		case Instruction::RTYPE:
			if(data.tokens()[0] == "add") {
                RETURN_RTYPE_INSTRUCTION(ADD);
			} else if(data.tokens()[0] == "sub") {
				RETURN_RTYPE_INSTRUCTION(SUB);
			} else if(data.tokens()[0] == "or") {
				RETURN_RTYPE_INSTRUCTION(OR);
			} else if(data.tokens()[0] == "and") {
				RETURN_RTYPE_INSTRUCTION(AND);
			}
		break;
		case Instruction::ADDI:
			RETURN_IMMEDIATE_INSTRUCTION(ADDI);
		case Instruction::BEQ:
			RETURN_IMMEDIATE_INSTRUCTION(BEQ);
        case Instruction::BNE:
            RETURN_IMMEDIATE_INSTRUCTION(BNE);
        case Instruction::ORI:
			RETURN_IMMEDIATE_INSTRUCTION(ORI);
		case Instruction::ANDI:
			RETURN_IMMEDIATE_INSTRUCTION(ANDI);
		case Instruction::LW:
			RETURN_MEMORY_INSTRUCTION(LW);
		case Instruction::SW:
			RETURN_MEMORY_INSTRUCTION(SW);
	}
	return nullptr;
}

} // namespace mips
