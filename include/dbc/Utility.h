#pragma once
#include <dbc/LeafNode.h>

namespace dbc
{
static bool IsBitwise(LeafPtr Node)
{
	if (Node != nullptr)
	{
		return Node->Type == LeafType::EXPR_XOR ||
			Node->Type == LeafType::EXPR_BOR ||
			Node->Type == LeafType::EXPR_BAND ||
			Node->Type == LeafType::EXPR_SHL ||
			Node->Type == LeafType::EXPR_SHR;
	}
	return false;
}
static bool IsLogical(LeafPtr Node)
{
	if (Node != nullptr)
	{
		return Node->Type == LeafType::EXPR_AND ||
			Node->Type == LeafType::EXPR_OR;
	}
	return false;
}
static bool IsCompare(LeafPtr Node)
{
	if (Node != nullptr)
	{
		return Node->Type == LeafType::EXPR_CMPEQU ||
			Node->Type == LeafType::EXPR_CMPGT ||
			Node->Type == LeafType::EXPR_CMPGTEQU ||
			Node->Type == LeafType::EXPR_CMPLT ||
			Node->Type == LeafType::EXPR_CMPLTEQU ||
			Node->Type == LeafType::EXPR_CMPNOTEQU;
	}
	return false;
}
static bool IsConst(LeafPtr Node)
{
	if (Node != nullptr)
	{
		return Node->Type == LeafType::CONST_BOOLEAN ||
			Node->Type == LeafType::CONST_IDENTIFIER ||
			Node->Type == LeafType::CONST_NUMBER;
	}
	return false;
}
static bool IsExpr(LeafPtr Node)
{
	if (Node != nullptr)
	{
		return (Node->Type == LeafType::EXPR_LIST ||
				Node->Type == LeafType::EXPR_XOR ||
				Node->Type == LeafType::EXPR_BOR ||
				Node->Type == LeafType::EXPR_SHL ||
				Node->Type == LeafType::EXPR_SHR ||
				Node->Type == LeafType::EXPR_BAND ||
				Node->Type == LeafType::EXPR_MOD ||
				Node->Type == LeafType::EXPR_OR ||
				Node->Type == LeafType::EXPR_AND ||
				Node->Type == LeafType::EXPR_NOT ||
				Node->Type == LeafType::EXPR_CMPEQU ||
				Node->Type == LeafType::EXPR_CMPGT ||
				Node->Type == LeafType::EXPR_CMPLT ||
				Node->Type == LeafType::EXPR_CMPNOTEQU ||
				Node->Type == LeafType::EXPR_CMPGTEQU ||
				Node->Type == LeafType::EXPR_CMPLTEQU ||
				Node->Type == LeafType::EXPR_ADD ||
				Node->Type == LeafType::EXPR_SUB ||
				Node->Type == LeafType::EXPR_DIV ||
				Node->Type == LeafType::EXPR_MUL ||
				Node->Type == LeafType::EXPR_NEGATE);
	}
	return false;
}
static bool IsExprGen(LeafPtr Node)
{
	if (Node != nullptr)
	{
		return (IsExpr(Node) ||
				Node->Type == LeafType::EXPR_CALL);
	}
	return false;
}
}