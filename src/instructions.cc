#include "instructions.h"

namespace mips {

Instruction::Register Instruction::RegisterNameToNumber(const std::string &name){
    static constexpr char register_strings[][4] = {"$at", "$v0", "$v1", "$a0",
                                                   "$a1", "$a2", "$a3", "$t0",
                                                   "$t1", "$t2", "$t3", "$t4",
                                                   "$t5", "$t6", "$t7", "$s0",
                                                   "$s1", "$s2", "$s3", "$s4",
                                                   "$s5", "$s6", "$s7", "$t8",
                                                   "$t9", "$k0", "$k1", "$gp",
                                                   "$sp", "$fp", "$ra"};

    static constexpr Instruction::Register registers[] = {AT, V0, V1, A0,
                                                          A1, A2, A3, T0,
                                                          T1, T2, T3, T4,
                                                          T5, T6, T7, S0,
                                                          S1, S2, S3, S4,
                                                          S5, S6, S7, T8,
                                                          T9, K0, K1, GP,
                                                          SP, FP, RA};

    static constexpr size_t n_registers = sizeof(registers) / sizeof(registers[0]);

    static_assert(n_registers == sizeof(register_strings) / sizeof(register_strings[0]));

    for(size_t i = 0; i < n_registers; ++i) {
        if(name == register_strings[i]) {
            return registers[i];
        }
    }
    return ZERO;
}

Instruction::~Instruction() {}

Instruction::Instruction(Instruction::Opcode opcode) : opcode_(opcode) {}

ImmediateInstruction::ImmediateInstruction(Instruction::Opcode opcode, Instruction::Register rt, Instruction::Register rs, uint16_t imm16) :
    Instruction(opcode), rs_(rs), rt_(rt), imm16_(imm16) {
    instruction_ = 0;
    instruction_ |= opcode;
    instruction_ |= (static_cast<uint32_t>(rt_) << 16u);
    instruction_ |= (static_cast<uint32_t>(rs_) << 21u);
    instruction_ |= imm16_;
}

uint32_t ImmediateInstruction::GetInstruction() const {
    return instruction_;
}

ADDIInstruction::ADDIInstruction(Instruction::Register rt, Instruction::Register rs, uint16_t imm16) :
    ImmediateInstruction(ADDI, rt, rs, imm16) {
}

ORIInstruction::ORIInstruction(Instruction::Register rt, Instruction::Register rs, uint16_t imm16) :
    ImmediateInstruction(ORI, rt, rs, imm16) {
}

ANDIInstruction::ANDIInstruction(Instruction::Register rt, Instruction::Register rs, uint16_t imm16) :
    ImmediateInstruction(ANDI, rt, rs, imm16) {
}

SLTIInstruction::SLTIInstruction(Instruction::Register rt, Instruction::Register rs, uint16_t imm16) :
    ImmediateInstruction(SLTI, rt, rs, imm16) {
}

BEQInstruction::BEQInstruction(Instruction::Register rt, Instruction::Register rs, uint16_t imm16) :
    ImmediateInstruction(BEQ, rt, rs, imm16) {
}

BNEInstruction::BNEInstruction(Instruction::Register rt, Instruction::Register rs, uint16_t imm16) :
    ImmediateInstruction(BNE, rt, rs, imm16) {
}

RTYPEInstruction::RTYPEInstruction(Instruction::Register rd, Instruction::Register rs, Instruction::Register rt, uint8_t shamt, uint8_t funct)
    : Instruction(RTYPE), rs_(rs), rt_(rt), rd_(rd), shamt_(shamt), funct_(funct) {
    instruction_ = 0;
    instruction_ |= opcode();
    instruction_ |= (static_cast<uint32_t>(rs_) << 21u);
    instruction_ |= (static_cast<uint32_t>(rt_) << 16u);
    instruction_ |= (static_cast<uint32_t>(rd_) << 11u);
    instruction_ |= (static_cast<uint32_t>(shamt_) << 6u);
    instruction_ |= funct_;
}

uint32_t RTYPEInstruction::GetInstruction() const { return instruction_; }

ADDInstruction::ADDInstruction(Instruction::Register rd, Instruction::Register rs, Instruction::Register rt, uint8_t shamt)
    : RTYPEInstruction(rd, rs, rt, shamt, 0x20) {}

SUBInstruction::SUBInstruction(Instruction::Register rd, Instruction::Register rs, Instruction::Register rt, uint8_t shamt)
    : RTYPEInstruction(rd, rs, rt, shamt, 0x22) {}

ANDInstruction::ANDInstruction(Instruction::Register rd, Instruction::Register rs, Instruction::Register rt, uint8_t shamt)
    : RTYPEInstruction(rd, rs, rt, shamt, 0x24) {}

ORInstruction::ORInstruction(Instruction::Register rd, Instruction::Register rs, Instruction::Register rt, uint8_t shamt)
    : RTYPEInstruction(rd, rs, rt, shamt, 0x25) {}

SLTInstruction::SLTInstruction(Instruction::Register rd, Instruction::Register rs, Instruction::Register rt, uint8_t shamt)
    : RTYPEInstruction(rd, rs, rt, shamt, 0x2a) {}

LWInstruction::LWInstruction(Instruction::Register rd, uint16_t imm16, Instruction::Register rt)
    : MemoryInstruction(LW, rd, rt, imm16) {}

SWInstruction::SWInstruction(Instruction::Register rd, uint16_t imm16, Instruction::Register rt)
    : MemoryInstruction(SW, rd, rt, imm16) {}

JumpInstruction::JumpInstruction(Instruction::Opcode opcode, uint32_t offset) : Instruction(opcode), offset_(offset) {
    instruction_ = static_cast<uint32_t>(opcode) | (offset_ & 0x03ffffffu);
}

uint32_t JumpInstruction::GetInstruction() const {
    return instruction_;
}

JALInstruction::JALInstruction(uint32_t offset) : JumpInstruction(JAL, offset) {}

JInstruction::JInstruction(uint32_t offset) : JumpInstruction(J, offset) {}

JRInstruction::JRInstruction(Instruction::Register reg) : RTYPEInstruction(ZERO, reg, ZERO, 0, 0x8) {}

} // namespace mips
