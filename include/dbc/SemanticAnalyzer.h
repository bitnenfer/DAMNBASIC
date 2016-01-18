#pragma once

#include <dbc/LeafNode.h>
#include <dbc/Scope.h>

namespace dbc
{
class SemanticAnalyzer
{
public:
	SemanticAnalyzer(const char* File, std::vector<Scope>* ScopeStack, bool IsMain = false);
	void Analyze(LeafPtr Tree);

protected:
	void AnalyzeNode(LeafPtr Current);
	bool CheckVarDecl(LeafPtr Current);
	bool CheckFuncDecl(LeafPtr Current);
	bool CheckAssign(LeafPtr Current);
	bool CheckFactor(LeafPtr Current);
	bool CheckExpr(LeafPtr Current);
	bool CheckCall(LeafPtr Current);
	bool CheckReturn(LeafPtr Current);
	bool CheckLHSnRHS(LeafPtr Current);
	bool CheckBase(LeafPtr Current);

	void EnterScope();
	void ExitScope();
	LeafPtr FindSymbol(LeafPtr Const);
	void AddSymbol(LeafPtr Node);
	bool SymbolExist(LeafPtr Node);

private:
	std::vector<Scope>* ScopeStack;
	Scope* CurrentScope;
	bool GotMain;
	const char* SourcePath;
	bool IsMain;
};
}