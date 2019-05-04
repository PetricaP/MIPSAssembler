#ifndef MIPS_H_
#define MIPS_H_

#include <cstdint>
#include <string>

namespace mips {

class Instruction {
public:
	enum Opcode {
		RTYPE = 0x00000000, // 0000 00 00
		ADDI  = 0x20000000, // 0010 00 00
		ANDI  = 0x30000000, // 0011 00 00
		ORI   = 0x34000000, // 0011 01 00
        SLTI  = 0x28000000, // 0010 10 00
        LW    = 0x8c000000, // 1000 11 00
		SW    = 0xac000000, // 1010 11 00
        BEQ   = 0x10000000, // 0001 00 00
        BNE   = 0x14000000, // 0001 01 00
        JAL   = 0x0c000000, // 0000 11 00
        J     = 0x08000000, // 0000 10 00
        JR    = 0x06000000  // 0000 01 00
    };

	enum Register {
		ZERO = 0,
		AT   = 1,
		V0   = 2,
		V1   = 3,
		A0   = 4,
		A1   = 5,
		A2   = 6,
		A3   = 7,
		T0   = 8,
		T1   = 9,
		T2   = 10,
		T3   = 11,
		T4   = 12,
		T5   = 13,
		T6   = 14,
		T7   = 15,
		S0   = 16,
		S1   = 17,
		S2   = 18,
		S3   = 19,
		S4   = 20,
		S5   = 21,
		S6   = 22,
		S7   = 23,
		T8   = 24,
		T9   = 25,
		K0   = 26,
		K1   = 27,
		GP   = 28,
		SP   = 29,
		FP   = 30,
        RA   = 31
	};

    static Register RegisterNameToNumber(std::string const &name);

public:
	virtual uint32_t GetInstruction() const = 0;
    virtual ~Instruction();

    Opcode opcode() const { return opcode_; }

protected:
    explicit Instruction(Opcode opcode);

private:
	Opcode opcode_;
};

class ImmediateInstruction : public Instruction {
public:
    uint32_t GetInstruction() const override;

    Register rt() const { return rt_; }
    Register rs() const { return rs_; }
    uint16_t imm16() const { return imm16_; }

protected:
    ImmediateInstruction(Opcode opcode, Register rt, Register rs, uint16_t imm16);

private:
    uint32_t instruction_;
    Register rs_;
    Register rt_;
    uint16_t imm16_;
};

class ADDIInstruction : public ImmediateInstruction {
public:
    ADDIInstruction(Register rt, Register rs, uint16_t imm16);
};

class ORIInstruction : public ImmediateInstruction {
public:
    ORIInstruction(Register rt, Register rs, uint16_t imm16);
};

class ANDIInstruction : public ImmediateInstruction {
public:
    ANDIInstruction(Register rt, Register rs, uint16_t imm16);
};

class SLTIInstruction : public ImmediateInstruction {
public:
    SLTIInstruction(Register rt, Register rs, uint16_t imm16);
};

class BEQInstruction : public ImmediateInstruction {
public:
    BEQInstruction(Register rt, Register rs, uint16_t imm16);
};

class BNEInstruction : public ImmediateInstruction {
public:
    BNEInstruction(Register rt, Register rs, uint16_t imm16);
};

class RTYPEInstruction : public Instruction {
public:
    Register rs() const { return rs_; }
    Register rt() const { return rt_; }
    Register rd() const { return rd_; }
    uint8_t shamt() const { return shamt_; }
    uint8_t funct() const { return funct_; }

    uint32_t GetInstruction() const override;

protected:
    RTYPEInstruction(Register rd, Register rs, Register rt, uint8_t shamt, uint8_t funct);

private:
	Register rs_;
	Register rt_;
	Register rd_;
	uint8_t shamt_;
	uint8_t funct_;
	uint32_t instruction_;
};

class ADDInstruction : public RTYPEInstruction {
public:
    ADDInstruction(Register rd, Register rs, Register rt, uint8_t shamt);
};

class SUBInstruction : public RTYPEInstruction {
public:
    SUBInstruction(Register rd, Register rs, Register rt, uint8_t shamt);
};

class ANDInstruction : public RTYPEInstruction {
public:
    ANDInstruction(Register rd, Register rs, Register rt, uint8_t shamt);
};

class ORInstruction : public RTYPEInstruction {
public:
    ORInstruction(Register rd, Register rs, Register rt, uint8_t shamt);
};

class SLTInstruction : public RTYPEInstruction {
public:
    SLTInstruction(Register rd, Register rs, Register rt, uint8_t shamt);
};

using MemoryInstruction = ImmediateInstruction;

class LWInstruction : public MemoryInstruction {
public:
    LWInstruction(Register rd, uint16_t imm16, Register rt);
};

class SWInstruction : public MemoryInstruction {
public:
    SWInstruction(Register rd, uint16_t imm16, Register rt);
};

class JumpInstruction : public Instruction {
public:
    uint32_t GetInstruction() const override;

protected:
    JumpInstruction(Opcode opcode, uint32_t offset);

private:
    uint32_t offset_;
    uint32_t instruction_;
};

class JALInstruction : public JumpInstruction {
public:
    JALInstruction(uint32_t offset);
};

class JInstruction : public JumpInstruction {
public:
    JInstruction(uint32_t offset);
};

class JRInstruction : public Instruction {
public:
    JRInstruction(Register reg);

    Register reg() { return reg_; }

    uint32_t GetInstruction() const override;

private:
    Register reg_;
    uint32_t instruction_;
};

}

#endif // MIPS_H_

