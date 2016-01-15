#pragma once

#include <string>
#include <dbc/Common.h>

namespace dbc
{

enum class TokenType
{
	LINEFEED,
	ENDOFFILE,
	WORD_OR,
	WORD_AND,
	WORD_NOT,
	WORD_FUNC,
	WORD_IF,
	WORD_ELSEIF,
	WORD_ELSE,
	WORD_LET,
	WORD_END,
	WORD_RET,
	WORD_WHILE,
	WORD_BAND,
	WORD_XOR,
	WORD_BOR,
	WORD_SHL,
	WORD_SHR,
	WORD_MOD,
	SYM_NOTEQU,
	SYM_EQU,
	SYM_LT,
	SYM_GT,
	SYM_LTEQU,
	SYM_GTEQU,
	SYM_PLUS,
	SYM_MINUS,
	SYM_SLASH,
	SYM_ASTERISK,
	SYM_COMMA,
	SYM_LPAREN,
	SYM_RPAREN,
	IDENTIFIER,
	NUMBER,
	BOOLEAN
}; // enum TokenType

struct Token
{
	Token(TokenType Type, uint64 Line) :
		Type(Type),
		Line(Line)
	{}
	TokenType Type;
	std::string RawValue;
	uint64 Line;
}; // struct Token
#if _DEBUG || DEBUG
using TokenPtr = Token*;
#else
using TokenPtr = std::shared_ptr<Token>;
#endif
} // namespace dbc
