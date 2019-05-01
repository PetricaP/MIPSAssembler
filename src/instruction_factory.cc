#include "instruction_factory.h"

namespace mips {

#define RETURN_RTYPE_INSTRUCTION(TYPE)\
			return std::make_unique<TYPE##Instruction>(\
						Instruction::RegisterNameToNumber(data.tokens()[1]),\
						Instruction::RegisterNameToNumber(data.tokens()[2]),\
						Instruction::RegisterNameToNumber(data.tokens()[3]),\
						0)

#define RETURN_IMEDIATE_INSTRUCTION(TYPE)\
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
			return std::make_unique<ADDInstruction>(\
						Instruction::RegisterNameToNumber(data.tokens()[1]),
						Instruction::RegisterNameToNumber(data.tokens()[2]),
						Instruction::RegisterNameToNumber(data.tokens()[3]),
						0);
			} else if(data.tokens()[0] == "sub") {
				RETURN_RTYPE_INSTRUCTION(SUB);
			} else if(data.tokens()[0] == "or") {
				RETURN_RTYPE_INSTRUCTION(OR);
			} else if(data.tokens()[0] == "and") {
				RETURN_RTYPE_INSTRUCTION(AND);
			}
		break;
		case Instruction::ADDI:
			RETURN_IMEDIATE_INSTRUCTION(ADDI);
		case Instruction::BEQ:
			RETURN_IMEDIATE_INSTRUCTION(BEQ);
        case Instruction::BNE:
            RETURN_IMEDIATE_INSTRUCTION(BNE);
        case Instruction::ORI:
			RETURN_IMEDIATE_INSTRUCTION(ORI);
		case Instruction::ANDI:
			RETURN_IMEDIATE_INSTRUCTION(ANDI);
		case Instruction::LW:
			RETURN_MEMORY_INSTRUCTION(LW);
		case Instruction::SW:
			RETURN_MEMORY_INSTRUCTION(SW);
	}
	return nullptr;
}

} // namespace mips
