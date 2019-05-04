#ifndef INSTRUCTION_FACTORY_H_
#define INSTRUCTION_FACTORY_H_

#include "instructions.h"
#include "parser.h"
#include <memory>

namespace mips {

class InstructionFactory {
public:
	static std::unique_ptr<Instruction> CreateInstruction(const Parser::InstructionData &data);
};

} // namespace mips

#endif // INSTRUCTION_FACTORY_H_
