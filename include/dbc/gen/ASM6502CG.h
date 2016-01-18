#pragma once

#include <dbc/CodeGenerator.h>
#include <unordered_map>

namespace dbc
{
namespace gen
{
class ASM6502CG : public CodeGenerator
{
public:
	ASM6502CG(bool Verbose = false);
	void LoadDependencies();
	virtual void Generate(LeafPtr Node, const char * File, bool IsMain = false) override;
	virtual std::string GenCode(LeafPtr Node) override;
	virtual std::string GenConst(LeafPtr Node) override;
	virtual std::string GenConstIdentifier(LeafPtr Node) override;
	virtual std::string GenConstBoolean(LeafPtr Node) override;
	virtual std::string GenConstNumber(LeafPtr Node) override;
	virtual std::string GenExpr(LeafPtr Node) override;
	virtual std::string GenExprNot(LeafPtr Node) override;
	virtual std::string GenExprCall(LeafPtr Node) override;
	virtual std::string GenExprNegate(LeafPtr Node) override;
	virtual std::string GenExprMul(LeafPtr Node) override;
	virtual std::string GenExprDiv(LeafPtr Node) override;
	virtual std::string GenExprAdd(LeafPtr Node) override;
	virtual std::string GenExprSub(LeafPtr Node) override;
	virtual std::string GenExprCompare(LeafPtr Node) override;
	virtual std::string GenExprLogical(LeafPtr Node) override;
	virtual std::string GenExprBitwise(LeafPtr Node) override;
	virtual std::string GenExprList(LeafPtr Node) override;
	virtual std::string GenDeclVar(LeafPtr Node) override;
	virtual std::string GenDeclFunc(LeafPtr Node) override;
	virtual std::string GenVarList(LeafPtr Node) override;
	virtual std::string GenStmtReturn(LeafPtr Node) override;
	virtual std::string GenStmtWhile(LeafPtr Node) override;
	virtual std::string GenStmtAssign(LeafPtr Node) override;
	virtual std::string GenStmtIf(LeafPtr Node) override;
	virtual std::string GenStmtElseIf(LeafPtr Node) override;
	virtual std::string GenStmtElse(LeafPtr Node) override;
	std::string GenExprCheck(LeafPtr Node);


protected:
	void AddGlobal(char* VarName);
	void AddLocal(char* VarName, bool Local);
	int16 GetGlobal(char* VarName, bool YieldError = true);
	int16 GetLocal(char* VarName);
	void EnterScope();
	void ExitScope();

private:
	bool Verbose = false;
	bool ScanningGlobals = false;
	bool InFunction = false;
	int16 GlobalAddr = 0x1010;
	std::unordered_map<std::string, int16> GlobalVariables;
	struct LocalVar
	{
		LocalVar(char* VarName, int16 Offset, bool Local) :
			VarName(VarName),
			Offset(Offset),
			Local(Local)
		{}
		char* VarName;
		int16 Offset;
		bool Local;
	};
	struct VarScope
	{
		std::vector<LocalVar> Variables;
	};
	std::vector<VarScope> Scopes;
	VarScope* CurrentScope = nullptr;
};
}
}