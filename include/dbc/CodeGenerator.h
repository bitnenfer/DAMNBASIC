#pragma once

#include <string>
#include <vector>
#include <dbc/LeafNode.h>
#include <dbc/Scope.h>
#include <algorithm>

namespace dbc
{
enum class GenType
{
	ASM6502_BUILD,
	ASM6502_SRC,
	NONE
};
class CodeGenerator
{
public:
	virtual void Generate(LeafPtr Node, const char* File, bool IsMain = false) = 0;
	virtual std::string GenCode(LeafPtr Node) = 0;
	virtual std::string GenConst(LeafPtr Node) = 0;
	virtual std::string GenConstIdentifier(LeafPtr Node) = 0;
	virtual std::string GenConstBoolean(LeafPtr Node) = 0;
	virtual std::string GenConstNumber(LeafPtr Node) = 0;
	virtual std::string GenExpr(LeafPtr Node) = 0;
	virtual std::string GenExprCall(LeafPtr Node) = 0;
	virtual std::string GenExprNegate(LeafPtr Node) = 0;
	virtual std::string GenExprMul(LeafPtr Node) = 0;
	virtual std::string GenExprDiv(LeafPtr Node) = 0;
	virtual std::string GenExprAdd(LeafPtr Node) = 0;
	virtual std::string GenExprSub(LeafPtr Node) = 0;
	virtual std::string GenExprNot(LeafPtr Node) = 0;
	virtual std::string GenExprCompare(LeafPtr Node) = 0;
	virtual std::string GenExprLogical(LeafPtr Node) = 0;
	virtual std::string GenExprBitwise(LeafPtr Node) = 0;
	virtual std::string GenExprList(LeafPtr Node) = 0;
	virtual std::string GenDeclVar(LeafPtr Node) = 0;
	virtual std::string GenDeclFunc(LeafPtr Node) = 0;
	virtual std::string GenVarList(LeafPtr Node) = 0;
	virtual std::string GenStmtReturn(LeafPtr Node) = 0;
	virtual std::string GenStmtWhile(LeafPtr Node) = 0;
	virtual std::string GenStmtAssign(LeafPtr Node) = 0;
	virtual std::string GenStmtIf(LeafPtr Node) = 0;
	virtual std::string GenStmtElseIf(LeafPtr Node) = 0;
	virtual std::string GenStmtElse(LeafPtr Node) = 0;
	virtual std::string GenMemAddressing(LeafPtr Node) = 0;
	inline std::string GetTransSource()
	{
		return Join();
	}
protected:
	inline std::string Join()
	{
		std::string Joined;
		for (std::string& Line : Lines)
		{
			Joined += Line + "\r";
		}
		return Joined;
	}
	inline void AddLine(std::string Line)
	{
		Lines.push_back(Line);
	}
	const char* CurrentFile;
	std::vector<std::string> Lines;
private:
	std::string Trans;
};
}