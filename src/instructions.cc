#include "instructions.h"

namespace mips {

Instruction::Register Instruction::RegisterNameToNumber(const std::string &name){
    if(name == "$zero") {
        return ZERO;
    } else if(name == "$at") {
        return AT;
    } else if(name == "$v0") {
        return V0;
    } else if(name == "$v1") {
        return V1;
    } else if(name == "$a0") {
        return A0;
    } else if(name == "$a1") {
        return A1;
    } else if(name =="$a2") {
        return A2;
    } else if(name =="$a3") {
        return A3;
    } else if(name =="$t0") {
        return T0;
    } else if(name =="$t1") {
        return T1;
    } else if(name =="$t2") {
        return T2;
    } else if(name =="$t3") {
        return T3;
    } else if(name =="$t4") {
        return T4;
    } else if(name =="$t5") {
        return T5;
    } else if(name =="$t6") {
        return T6;
    } else if(name =="$t7") {
        return T7;
    } else if(name =="$s0") {
        return S0;
    } else if(name =="$s1") {
        return S1;
    } else if(name =="$s2") {
        return S2;
    } else if(name =="$s3") {
        return S3;
    } else if(name =="$s4") {
        return S4;
    } else if(name =="$s5") {
        return S5;
    } else if(name =="$s6") {
        return S6;
    } else if(name =="$s7") {
        return S7;
    } else if(name =="$t8") {
        return T8;
    } else if(name =="$t9") {
        return T9;
    } else if(name =="$k0") {
        return K0;
    } else if(name =="$k1") {
        return K1;
    } else if(name =="$gp") {
        return GP;
    } else if(name =="$sp") {
        return SP;
    } else if(name =="$fp") {
        return FP;
    } else if(name =="$ra") {
        return RA;
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

JRInstruction::JRInstruction(Instruction::Register reg) : Instruction(JR) {
    instruction_ = opcode() | (static_cast<uint32_t>(reg) << 20u) | 0x8u;
}

uint32_t JRInstruction::GetInstruction() const {
    return instruction_;
}

} // namespace mips
