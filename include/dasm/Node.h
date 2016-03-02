#pragma once
#include <memory>
#include <string>
#include <dasm/Types.h>
#include <dasm/OpCodes.h>

namespace dasm
{
struct Node;
#if _DEBUG || DEBUG
using NodePtr = Node*;
#else
using NodePtr = std::shared_ptr<Node>;
#endif

enum class ENode
{
	ORIGIN,
	LABEL,
	ARG_LIST,
	DECL_VAR,
	ADDR_A,
	ADDR_IMM,
	ADDR_INDX,
	ADDR_INDY,
	ADDR_IND,
	ADDR_ZPY,
	ADDR_ZPX,
	ADDR_ABSY,
	ADDR_ABSX,
	ADDR_ZP,
	ADDR_AB,
	ADDR_IMP,
	INSTRUCTION,
	LINE,
	ENDOFFILE,
	DIR_WORD,
	DIR_BYTE,
	DIR_INCLUDE,
	HEX_ADDRESS,
	IDE_ADDRESS,
	HEX_VALUE,
	DEC_VALUE,
	IDE_VALUE,
	STR_VALUE,
};

struct Node
{
	Node(ENode Type, uint32 Line) :
		Type(Type),
		Line(Line),
		Left(nullptr),
		Right(nullptr),
		OpCode(EOpCode::NOOP),
		AddrMode(EAddressMode::IMP),
		IsHex(false),
		Is8Bit(false),
		UINT8(0)
	{}
	ENode Type;
	uint32 Line;
	NodePtr Left;
	NodePtr Right;
	EOpCode OpCode;
	EAddressMode AddrMode;
	bool IsHex;
	bool Is8Bit;
	union
	{
		uint8 UINT8;
		uint16 UINT16;
	};
	std::string STRING;
};
}