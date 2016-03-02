#pragma once

#include <dasm/Common.h>

namespace dasm
{
enum class EToken
{
	NONE,
	MNEMONIC,
	SYM_PLUS,
	SYM_MINUS,
	SYM_ASTERISK,
	SYM_SLASH,
	SYM_EQUALS,
	SYM_LPAREN,
	SYM_RPAREN,
	SYM_PERCENT,
	SYM_LT,
	SYM_GT,
	SYM_SEMI,
	SYM_COLON,
	SYM_HASH,
	SYM_DOLLAR,
	SYM_COMMA,
	WORD_BYTE,
	WORD_WORD,
	WORD_INCLUDE,
	SYM_X,
	SYM_Y,
	SYM_A,
	HEX_VALUE,
	HEX_ADDRESS,
	DEC_VALUE,
	IDENT_VALUE,
	IDENT_ADDRESS,
	STRING
};
struct Token
{
	Token(EToken Type, uint32 Line) :
		Type(Type),
		Line(Line)
	{}
	EToken Type;
	std::string RawValue;
	uint32 Line;
};
#if _DEBUG || DEBUG
using TokenPtr = Token*;
#else
using TokenPtr = std::shared_ptr<Token>;
#endif
}
