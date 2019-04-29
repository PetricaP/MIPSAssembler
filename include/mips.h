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
		LW    = 0x8c000000, // 1000 11 00
		SW    = 0xac000000, // 1010 11 00
		BEQ   = 0x10000000  // 0001 00 00
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
		RA   = 30
	};

	static Register RegisterNameToNumber(std::string const &name){
		if(name == "$zero") {
			return ZERO;
		} else if(name == "$at") {
			return AT;
		} else if(name == "$v0") {
			return V0;
		} else if(name == "$v1") {
			return V1;
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

public:
	virtual uint32_t GetInstruction() const = 0;
	virtual ~Instruction() {}

	Opcode opcode() const { return opcode_; }

protected:
	Instruction(Opcode opcode) : opcode_(opcode) {}

private:
	Opcode opcode_;
};

class ImmediateInstruction : public Instruction {
public:
	ImmediateInstruction(Opcode opcode, Register rt, Register rs, uint16_t imm16) :
		Instruction(opcode), rt_(rt), rs_(rs), imm16_(imm16) {
		instruction_ = 0;
		instruction_ |= opcode;
		instruction_ |= (static_cast<uint32_t>(rt_) << 16);
		instruction_ |= (static_cast<uint32_t>(rs_) << 21);
		instruction_ |= imm16_;
	}

	uint32_t GetInstruction() const override {
		return instruction_;
	}

	Register rt() const { return rt_; }
	Register rs() const { return rs_; }
	uint16_t imm16() const { return imm16_; }

private:
	uint32_t instruction_;
	Register rt_;
	Register rs_;
	uint16_t imm16_;
};

class ADDIInstruction : public ImmediateInstruction {
public:
	ADDIInstruction(Register rt, Register rs, uint16_t imm16) :
		ImmediateInstruction(ADDI, rt, rs, imm16) {
	}
};

class ORIInstruction : public ImmediateInstruction {
public:
	ORIInstruction(Register rt, Register rs, uint16_t imm16) :
		ImmediateInstruction(ORI, rt, rs, imm16) {
	}
};

class ANDIInstruction : public ImmediateInstruction {
public:
	ANDIInstruction(Register rt, Register rs, uint16_t imm16) :
		ImmediateInstruction(ANDI, rt, rs, imm16) {
	}
};

class BEQInstruction : public ImmediateInstruction {
public:
	BEQInstruction(Register rt, Register rs, uint16_t imm16) :
		ImmediateInstruction(BEQ, rt, rs, imm16) {
	}
};

class RTYPEInstruction : public Instruction {
protected:
	RTYPEInstruction(Register rd, Register rs, Register rt, uint8_t shamt, uint8_t funct)
		: Instruction(RTYPE), rs_(rs), rt_(rt), rd_(rd), shamt_(shamt), funct_(funct) {
		instruction_ = 0;
		instruction_ |= opcode();
		instruction_ |= (static_cast<uint32_t>(rs_) << 21);
		instruction_ |= (static_cast<uint32_t>(rt_) << 16);
		instruction_ |= (static_cast<uint32_t>(rd_) << 11);
		instruction_ |= (static_cast<uint32_t>(shamt_) << 6);
		instruction_ |= funct_;
	}
public:
	Register rs() const { return rs_; }
	Register rt() const { return rt_; }
	Register rd() const { return rd_; }
	uint8_t shamt() const { return shamt_; }
	uint8_t funct() const { return funct_; }

	uint32_t GetInstruction() const override { return instruction_; }

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
	ADDInstruction(Register rd, Register rs, Register rt, uint8_t shamt)
		: RTYPEInstruction(rd, rs, rt, shamt, 0x20) {}
};

class SUBInstruction : public RTYPEInstruction {
public:
	SUBInstruction(Register rd, Register rs, Register rt, uint8_t shamt)
		: RTYPEInstruction(rd, rs, rt, shamt, 0x22) {}
};

class ANDInstruction : public RTYPEInstruction {
public:
	ANDInstruction(Register rd, Register rs, Register rt, uint8_t shamt)
		: RTYPEInstruction(rd, rs, rt, shamt, 0x24) {}
};

class ORInstruction : public RTYPEInstruction {
public:
	ORInstruction(Register rd, Register rs, Register rt, uint8_t shamt)
		: RTYPEInstruction(rd, rs, rt, shamt, 0x25) {}
};

class SLTInstruction : public RTYPEInstruction {
public:
	SLTInstruction(Register rd, Register rs, Register rt, uint8_t shamt)
		: RTYPEInstruction(rd, rs, rt, shamt, 0x2a) {}
};

using MemoryInstruction = ImmediateInstruction;

class LWInstruction : public MemoryInstruction {
public:
	LWInstruction(Register rd, uint16_t imm16, Register rt)
		: MemoryInstruction(LW, rd, rt, imm16) {}
};

class SWInstruction : public MemoryInstruction {
public:
	SWInstruction(Register rd, uint16_t imm16, Register rt)
		: MemoryInstruction(SW, rd, rt, imm16) {}
};

}

#endif // MIPS_H_
