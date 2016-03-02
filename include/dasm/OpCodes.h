#pragma once
#include <dasm/Common.h>
#include <dasm/Types.h>
namespace dasm
{
constexpr uint8 OpCodeTableOffset = 13;
constexpr uint8 NOOP = 0xFF;
constexpr uint8 OpCodeTable[] =
{
	0x69, 0x65, 0x75, NOOP, 0x6d, 0x7d, 0x79, NOOP, 0x61, 0x71, NOOP, NOOP, NOOP,
	0x29, 0x25, 0x35, NOOP, 0x2d, 0x3d, 0x39, NOOP, 0x21, 0x31, NOOP, NOOP, NOOP,
	NOOP, 0x06, 0x16, NOOP, 0x0e, 0x1e, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, 0x0a,
	NOOP, 0x24, NOOP, NOOP, 0x2c, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP,
	NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, 0x10, NOOP,
	NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, 0x30, NOOP,
	NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, 0x50, NOOP,
	NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, 0x70, NOOP,
	NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, 0x90, NOOP,
	NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, 0xb0, NOOP,
	NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, 0xd0, NOOP,
	NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, 0xf0, NOOP,
	NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, 0x00, NOOP, NOOP,
	0xc9, 0xc5, 0xd5, NOOP, 0xcd, 0xdd, 0xd9, NOOP, 0xc1, 0xd1, NOOP, NOOP, NOOP,
	0xe0, 0xe4, NOOP, NOOP, 0xec, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP,
	0xc0, 0xc4, NOOP, NOOP, 0xcc, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP,
	NOOP, 0xc6, 0xd6, NOOP, 0xce, 0xde, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP,
	0x49, 0x45, 0x55, NOOP, 0x4d, 0x5d, 0x59, NOOP, 0x41, 0x51, NOOP, NOOP, NOOP,
	NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, 0x18, NOOP, NOOP,
	NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, 0x38, NOOP, NOOP,
	NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, 0x58, NOOP, NOOP,
	NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, 0x78, NOOP, NOOP,
	NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, 0xb8, NOOP, NOOP,
	NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, 0xd8, NOOP, NOOP,
	NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, 0xf8, NOOP, NOOP,
	NOOP, 0xe6, 0xf6, NOOP, 0xee, 0xfe, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP,
	NOOP, NOOP, NOOP, NOOP, 0x4c, NOOP, NOOP, 0x6c, NOOP, NOOP, NOOP, NOOP, NOOP,
	NOOP, NOOP, NOOP, NOOP, 0x20, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP,
	0xa9, 0xa5, 0xb5, NOOP, 0xad, 0xbd, 0xb9, NOOP, 0xa1, 0xb1, NOOP, NOOP, NOOP,
	0xa2, 0xa6, NOOP, 0xb6, 0xae, NOOP, 0xbe, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP,
	0xa0, 0xa4, 0xb4, NOOP, 0xac, 0xbc, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP,
	NOOP, 0x46, 0x56, NOOP, 0x4e, 0x5e, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, 0x4a,
	NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, 0xea, NOOP, NOOP,
	0x09, 0x05, 0x15, NOOP, 0x0d, 0x1d, 0x19, NOOP, 0x01, 0x11, NOOP, NOOP, NOOP,
	NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, 0xaa, NOOP, NOOP,
	NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, 0x8a, NOOP, NOOP,
	NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, 0xca, NOOP, NOOP,
	NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, 0xe8, NOOP, NOOP,
	NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, 0xa8, NOOP, NOOP,
	NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, 0x98, NOOP, NOOP,
	NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, 0x88, NOOP, NOOP,
	NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, 0xc8, NOOP, NOOP,
	NOOP, 0x66, 0x76, NOOP, 0x6e, 0x7e, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, 0x6a,
	NOOP, 0x26, 0x36, NOOP, 0x2e, 0x3e, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, 0x2a,
	NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, 0x40, NOOP, NOOP,
	NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, 0x60, NOOP, NOOP,
	0xe9, 0xe5, 0xf5, NOOP, 0xed, 0xfd, 0xf9, NOOP, 0xe1, 0xf1, NOOP, NOOP, NOOP,
	NOOP, 0x85, 0x95, NOOP, 0x8d, 0x9d, 0x99, NOOP, 0x81, 0x91, NOOP, NOOP, NOOP,
	NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, 0x9a, NOOP, NOOP,
	NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, 0xba, NOOP, NOOP,
	NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, 0x48, NOOP, NOOP,
	NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, 0x68, NOOP, NOOP,
	NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, 0x08, NOOP, NOOP,
	NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, 0x28, NOOP, NOOP,
	NOOP, 0x86, NOOP, 0x96, 0x8e, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP,
	NOOP, 0x84, 0x94, NOOP, 0x8c, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP
};
enum class EAddressMode : uint8
{
	IMM,
	ZP,
	ZPX,
	ZPY,
	AB,
	ABX,
	ABY,
	IND,
	IDX,
	IDY,
	IMP,
	REL,
	A
};
enum class EOpCode : uint8
{
	ADC,
	AND,
	ASL,
	BIT,
	BPL,
	BMI,
	BVC,
	BVS,
	BCC,
	BCS,
	BNE,
	BEQ,
	BRK,
	CMP,
	CPX,
	CPY,
	DEC,
	EOR,
	CLC,
	SEC,
	CLI,
	SEI,
	CLV,
	CLD,
	SED,
	INC,
	JMP,
	JSR,
	LDA,
	LDX,
	LDY,
	LSR,
	NOP,
	ORA,
	TAX,
	TXA,
	DEX,
	INX,
	TAY,
	TYA,
	DEY,
	INY,
	ROR,
	ROL,
	RTI,
	RTS,
	SBC,
	STA,
	TXS,
	TSX,
	PHA,
	PLA,
	PHP,
	PLP,
	STX,
	STY,
	NOOP
};	

static const EOpCode Branches[] = 
{
	EOpCode::BCC,
	EOpCode::BCS,
	EOpCode::BEQ,
	EOpCode::BMI,
	EOpCode::BNE,
	EOpCode::BPL,
	EOpCode::BVC,
	EOpCode::BVS
};

static const char* AddrModeToString(EAddressMode Mode)
{
	if (Mode == EAddressMode::IMM) return "IMM";
	if (Mode == EAddressMode::ZP) return "ZP";
	if (Mode == EAddressMode::ZPX) return "ZPX";
	if (Mode == EAddressMode::ZPY) return "ZPY";
	if (Mode == EAddressMode::AB) return "AB";
	if (Mode == EAddressMode::ABX) return "ABX";
	if (Mode == EAddressMode::ABY) return "ABY";
	if (Mode == EAddressMode::IND) return "IND";
	if (Mode == EAddressMode::IDX) return "IDX";
	if (Mode == EAddressMode::IDY) return "IDY";
	if (Mode == EAddressMode::IMP) return "IMP";
	if (Mode == EAddressMode::REL) return "REL";
	if (Mode == EAddressMode::A) return "A";
	return "None";
}

static const char* OpCodeToString(EOpCode Type)
{
	if (Type == EOpCode::ADC) return "ADC";
	if (Type == EOpCode::AND) return "AND";
	if (Type == EOpCode::ASL) return "ASL";
	if (Type == EOpCode::BIT) return "BIT";
	if (Type == EOpCode::BPL) return "BPL";
	if (Type == EOpCode::BMI) return "BMI";
	if (Type == EOpCode::BVC) return "BVC";
	if (Type == EOpCode::BVS) return "BVS";
	if (Type == EOpCode::BCC) return "BCC";
	if (Type == EOpCode::BCS) return "BCS";
	if (Type == EOpCode::BNE) return "BNE";
	if (Type == EOpCode::BEQ) return "BEQ";
	if (Type == EOpCode::BRK) return "BRK";
	if (Type == EOpCode::CMP) return "CMP";
	if (Type == EOpCode::CPX) return "CPX";
	if (Type == EOpCode::CPY) return "CPY";
	if (Type == EOpCode::DEC) return "DEC";
	if (Type == EOpCode::EOR) return "EOR";
	if (Type == EOpCode::CLC) return "CLC";
	if (Type == EOpCode::SEC) return "SEC";
	if (Type == EOpCode::CLI) return "CLI";
	if (Type == EOpCode::SEI) return "SEI";
	if (Type == EOpCode::CLV) return "CLV";
	if (Type == EOpCode::CLD) return "CLD";
	if (Type == EOpCode::SED) return "SED";
	if (Type == EOpCode::INC) return "INC";
	if (Type == EOpCode::JMP) return "JMP";
	if (Type == EOpCode::JSR) return "JSR";
	if (Type == EOpCode::LDA) return "LDA";
	if (Type == EOpCode::LDX) return "LDX";
	if (Type == EOpCode::LDY) return "LDY";
	if (Type == EOpCode::LSR) return "LSR";
	if (Type == EOpCode::NOP) return "NOP";
	if (Type == EOpCode::ORA) return "ORA";
	if (Type == EOpCode::TAX) return "TAX";
	if (Type == EOpCode::TXA) return "TXA";
	if (Type == EOpCode::DEX) return "DEX";
	if (Type == EOpCode::INX) return "INX";
	if (Type == EOpCode::TAY) return "TAY";
	if (Type == EOpCode::TYA) return "TYA";
	if (Type == EOpCode::DEY) return "DEY";
	if (Type == EOpCode::INY) return "INY";
	if (Type == EOpCode::ROR) return "ROR";
	if (Type == EOpCode::ROL) return "ROL";
	if (Type == EOpCode::RTI) return "RTI";
	if (Type == EOpCode::RTS) return "RTS";
	if (Type == EOpCode::SBC) return "SBC";
	if (Type == EOpCode::STA) return "STA";
	if (Type == EOpCode::TXS) return "TXS";
	if (Type == EOpCode::TSX) return "TSX";
	if (Type == EOpCode::PHA) return "PHA";
	if (Type == EOpCode::PLA) return "PLA";
	if (Type == EOpCode::PHP) return "PHP";
	if (Type == EOpCode::PLP) return "PLP";
	if (Type == EOpCode::STX) return "STX";
	if (Type == EOpCode::STY) return "STY";
	return "NOOP";
}
static EOpCode GetOpCode(std::string RawOp)
{
	if (RawOp == "ADC") return EOpCode::ADC;
	if (RawOp == "AND") return EOpCode::AND;
	if (RawOp == "ASL") return EOpCode::ASL;
	if (RawOp == "BIT") return EOpCode::BIT;
	if (RawOp == "BPL") return EOpCode::BPL;
	if (RawOp == "BMI") return EOpCode::BMI;
	if (RawOp == "BVC") return EOpCode::BVC;
	if (RawOp == "BVS") return EOpCode::BVS;
	if (RawOp == "BCC") return EOpCode::BCC;
	if (RawOp == "BCS") return EOpCode::BCS;
	if (RawOp == "BNE") return EOpCode::BNE;
	if (RawOp == "BEQ") return EOpCode::BEQ;
	if (RawOp == "BRK") return EOpCode::BRK;
	if (RawOp == "CMP") return EOpCode::CMP;
	if (RawOp == "CPX") return EOpCode::CPX;
	if (RawOp == "CPY") return EOpCode::CPY;
	if (RawOp == "DEC") return EOpCode::DEC;
	if (RawOp == "EOR") return EOpCode::EOR;
	if (RawOp == "CLC") return EOpCode::CLC;
	if (RawOp == "SEC") return EOpCode::SEC;
	if (RawOp == "CLI") return EOpCode::CLI;
	if (RawOp == "SEI") return EOpCode::SEI;
	if (RawOp == "CLV") return EOpCode::CLV;
	if (RawOp == "CLD") return EOpCode::CLD;
	if (RawOp == "SED") return EOpCode::SED;
	if (RawOp == "INC") return EOpCode::INC;
	if (RawOp == "JMP") return EOpCode::JMP;
	if (RawOp == "JSR") return EOpCode::JSR;
	if (RawOp == "LDA") return EOpCode::LDA;
	if (RawOp == "LDX") return EOpCode::LDX;
	if (RawOp == "LDY") return EOpCode::LDY;
	if (RawOp == "LSR") return EOpCode::LSR;
	if (RawOp == "NOP") return EOpCode::NOP;
	if (RawOp == "ORA") return EOpCode::ORA;
	if (RawOp == "TAX") return EOpCode::TAX;
	if (RawOp == "TXA") return EOpCode::TXA;
	if (RawOp == "DEX") return EOpCode::DEX;
	if (RawOp == "INX") return EOpCode::INX;
	if (RawOp == "TAY") return EOpCode::TAY;
	if (RawOp == "TYA") return EOpCode::TYA;
	if (RawOp == "DEY") return EOpCode::DEY;
	if (RawOp == "INY") return EOpCode::INY;
	if (RawOp == "ROR") return EOpCode::ROR;
	if (RawOp == "ROL") return EOpCode::ROL;
	if (RawOp == "RTI") return EOpCode::RTI;
	if (RawOp == "RTS") return EOpCode::RTS;
	if (RawOp == "SBC") return EOpCode::SBC;
	if (RawOp == "STA") return EOpCode::STA;
	if (RawOp == "TXS") return EOpCode::TXS;
	if (RawOp == "TSX") return EOpCode::TSX;
	if (RawOp == "PHA") return EOpCode::PHA;
	if (RawOp == "PLA") return EOpCode::PLA;
	if (RawOp == "PHP") return EOpCode::PHP;
	if (RawOp == "PLP") return EOpCode::PLP;
	if (RawOp == "STX") return EOpCode::STX;
	if (RawOp == "STY") return EOpCode::STY;
	return EOpCode::NOOP;
}
inline static uint8 GetAddress(EOpCode OpCode, EAddressMode AddressMode)
{
	uint32 OpCodeInt = static_cast<uint32>(OpCode);
	uint32 OpAddress = (OpCodeInt * OpCodeTableOffset);
	uint32 AddressModeOffset = static_cast<uint32>(AddressMode);
	return OpCodeTable[OpAddress + AddressModeOffset];
}
inline static bool IsImplicit(EOpCode OpCode)
{
	return GetAddress(OpCode, EAddressMode::IMP) != NOOP;
}
static bool IsBranch(EOpCode OpCode)
{
	return GetAddress(OpCode, EAddressMode::REL) != NOOP;
}
}