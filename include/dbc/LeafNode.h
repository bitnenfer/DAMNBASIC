#pragma once
#include <dbc/Common.h>

namespace dbc
{
enum class LeafType
{
	PROGRAM,
	STMT_SEQ,
	STMT,
	STMT_IF,
	STMT_ELSEIF,
	STMT_ELSE,
	STMT_ASSIGN,
	STMT_MEMWRITE,
	STMT_WHILE,
	STMT_RETURN,
	DECL_VAR,
	DECL_HEAD,
	DECL_FUNC,
	DECL_VARLIST,
	EXPR_MEMREAD,
	EXPR_LIST,
	EXPR_XOR,
	EXPR_BOR,
	EXPR_SHL,
	EXPR_SHR,
	EXPR_BAND,
	EXPR_MOD,
	EXPR_OR,
	EXPR_AND,
	EXPR_NOT,
	EXPR_CMPEQU,
	EXPR_CMPGT,
	EXPR_CMPLT,
	EXPR_CMPNOTEQU,
	EXPR_CMPGTEQU,
	EXPR_CMPLTEQU,
	EXPR_ADD,
	EXPR_SUB,
	EXPR_DIV,
	EXPR_MUL,
	EXPR_NEGATE,
	EXPR_CALL,
	CONST_MEMADDRESS,
	CONST_NUMBER,
	CONST_BOOLEAN,
	CONST_IDENTIFIER,
	ENDOFFILE
};

struct LeafNode;
#if _DEBUG || DEBUG
using LeafPtr = LeafNode*;
#else
using LeafPtr = std::shared_ptr<LeafNode>;
#endif

struct LeafNode
{
	LeafNode(LeafType Type, uint64 Line) :
		Type(Type),
		Left(nullptr),
		Right(nullptr),
		Line(Line),
		UINT8(0)
	{}
	LeafType Type;
	LeafPtr Left;
	LeafPtr Right;
	uint64 Line;
	union
	{
		uint16 UINT16;
		uint8 UINT8;
		bool BOOL;
		char* STRING;
	};
};

}