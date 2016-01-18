#include <dbc/SemanticAnalyzer.h>
#include <dbc/Utility.h>

#define SemantError(Message, ...) \
	fprintf(stderr, "\n%s ", SourcePath); \
	if (Current != nullptr) fprintf(stderr, "Line: %zu - Semantic Error: ", Current->Line);\
	fprintf(stderr, Message, ##__VA_ARGS__); \
	getchar(); \
	exit(-1);

using namespace dbc;

dbc::SemanticAnalyzer::SemanticAnalyzer(const char * File, std::vector<Scope>* ScopeStack, bool IsMain) :
	CurrentScope(nullptr),
	SourcePath(File),
	IsMain(IsMain),
	ScopeStack(ScopeStack)
{}

void dbc::SemanticAnalyzer::Analyze(LeafPtr Tree)
{
	if (IsMain)
	{
		EnterScope();
	}
	LeafPtr Current = Tree;
	while (Current != nullptr && Current->Type != LeafType::ENDOFFILE)
	{
		AnalyzeNode(Current->Left);
		if (Current->Right != nullptr)
		{
			Current = Current->Right;
		}
		else
		{
			break;
		}
	}
	if (IsMain)
	{
		ExitScope();
	}
	if (!GotMain && IsMain)
	{
		SemantError("Missing entry point function 'main'");
	}
}

#pragma region Scope related methods
void dbc::SemanticAnalyzer::AnalyzeNode(LeafPtr Current)
{
	if (Current != nullptr)
	{
		if (Current->Type == LeafType::DECL_FUNC)
		{
			if (!CheckFuncDecl(Current))
			{
				SemantError("Invalid function declaration");
			}
		}
		else if (Current->Type == LeafType::STMT_WHILE)
		{
			if (!CheckExpr(Current->Left))
			{
				SemantError("Invalid expression");
			}
			EnterScope();
			AnalyzeNode(Current->Right);
			ExitScope();
		}
		else if (Current->Type == LeafType::STMT_IF ||
				 Current->Type == LeafType::STMT_ELSEIF)
		{
			if (IsExpr(Current->Left) && !CheckExpr(Current->Left))
			{
				SemantError("Invalid expression");
			}
			else
			{
				CheckFactor(Current->Left);
			}
			EnterScope();
			AnalyzeNode(Current->Right);
			ExitScope();
		}
		else if (Current->Type == LeafType::STMT_ELSE)
		{
			EnterScope();
			AnalyzeNode(Current->Left);
			ExitScope();
		}
		else if (Current->Type == LeafType::DECL_VAR)
		{
			if (!CheckVarDecl(Current))
			{
				SemantError("Invalid variable declaration");
			}
			else
			{
				AddSymbol(Current);
			}
		}
		else if (Current->Type == LeafType::STMT_ASSIGN)
		{
			if (!CheckAssign(Current))
			{
				SemantError("Invalid assignment");
			}
		}
		else if (Current->Type == LeafType::STMT)
		{
			AnalyzeNode(Current->Left);
			AnalyzeNode(Current->Right);
		}
		else if (Current->Type == LeafType::EXPR_CALL)
		{
			if (!CheckCall(Current))
			{
				SemantError("Invalid function call");
			}
		}
		else if (Current->Type == LeafType::STMT_RETURN)
		{
			if (!CheckReturn(Current))
			{
				SemantError("Invalid return statement");
			}
		}
		else if (IsBitwise(Current) ||
				 IsLogical(Current) || 
				 Current->Type == LeafType::EXPR_MOD)
		{
			if (!CheckLHSnRHS(Current))
			{
				SemantError("Invalid Expression");
			}
		}
	}
}
bool dbc::SemanticAnalyzer::CheckVarDecl(LeafPtr Current)
{
	if (IsExpr(Current->Right))
	{
		return CheckExpr(Current->Right);
	}
	else if (IsConst(Current->Right))
	{
		return CheckFactor(Current->Right);
	}
	else if (Current->Right->Type == LeafType::EXPR_CALL)
	{
		return CheckCall(Current->Right);
	}
	return false;
}
bool dbc::SemanticAnalyzer::CheckFuncDecl(LeafPtr Current)
{
	AddSymbol(Current);
	EnterScope();
	if (IsMain)
	{
		if (strcmp(Current->Left->Left->STRING, "main") == 0)
		{
			GotMain = true;
		}
	}
	if (Current->Left->Right != nullptr &&
		Current->Left->Right->Type == LeafType::DECL_VARLIST)
	{
		LeafPtr VarList = Current->Left->Right;
		while (VarList != nullptr && VarList->Type == LeafType::DECL_VARLIST)
		{
			AddSymbol(VarList->Left);
			VarList = VarList->Right;
		}
	}
	if (Current->Right != nullptr)
	{
		AnalyzeNode(Current->Right);
	}
	ExitScope();
	return true;
}
bool dbc::SemanticAnalyzer::CheckAssign(LeafPtr Current)
{
	CheckFactor(Current->Left);
	if (IsExpr(Current->Right) && !CheckExpr(Current->Right))
	{
		SemantError("Invalid Expression");
	}
	else if (Current->Right->Type == LeafType::CONST_IDENTIFIER ||
			 Current->Right->Type == LeafType::EXPR_CALL)
	{
		return CheckFactor(Current->Right);
	}
	return true;
}
bool dbc::SemanticAnalyzer::CheckFactor(LeafPtr Current)
{
	if (Current != nullptr)
	{
		if (Current->Type == LeafType::CONST_IDENTIFIER && !SymbolExist(Current))
		{
			SemantError("Undefined variable %s", Current->STRING);
			return false;
		}
		else if (Current->Type == LeafType::EXPR_CALL && !CheckCall(Current))
		{
			SemantError("Undefined function %s", Current->Left->STRING);
			return false;
		}
	}
	return true;
}
bool dbc::SemanticAnalyzer::CheckExpr(LeafPtr Current)
{
	if (Current->Left != nullptr)
	{
		if (IsExpr(Current->Left))
		{
			CheckExpr(Current->Left);
		}
		else
		{
			CheckFactor(Current->Left);
		}
	}
	if (Current->Right != nullptr)
	{
		if (IsExpr(Current->Right))
		{
			CheckExpr(Current->Right);
		}
		else
		{
			CheckFactor(Current->Right);
		}
	}
	return true;
}
bool dbc::SemanticAnalyzer::CheckCall(LeafPtr Current)
{
	LeafPtr FuncDecl = FindSymbol(Current);
	if (FuncDecl == nullptr)
	{
		SemantError("Calling undefined function %s", Current->Left->Left->STRING);
	}
	uint64 DeclCount = 0;
	uint64 CallCount = 0;
	if (FuncDecl->Left->Right != nullptr)
	{
		LeafPtr VarList = FuncDecl->Left->Right;
		while (VarList != nullptr && VarList->Type == LeafType::DECL_VARLIST)
		{
			if (VarList->Left != nullptr)
			{
				++DeclCount;
			}
			VarList = VarList->Right;
		}
	}
	if (Current->Right != nullptr)
	{
		LeafPtr ExprList = Current->Right;
		while (ExprList != nullptr && ExprList->Type == LeafType::EXPR_LIST)
		{
			if (ExprList->Left != nullptr)
			{
				CheckFactor(ExprList->Left);
				++CallCount;
			}
			ExprList = ExprList->Right;
		}
	}
	if (DeclCount != CallCount)
	{
		SemantError("Incorrect number of arguments. Function %s requires %lu argument/s. %lu were provided.", 
					FuncDecl->Left->Left->STRING, static_cast<long unsigned int>(DeclCount), static_cast<long unsigned int>(CallCount));
	}
	return true;
}
bool dbc::SemanticAnalyzer::CheckReturn(LeafPtr Current)
{
	return CheckBase(Current->Left);
}
bool dbc::SemanticAnalyzer::CheckLHSnRHS(LeafPtr Current)
{
	if (CheckBase(Current->Left))
	{
		return (CheckBase(Current->Right));
	}
	return false;
}
bool dbc::SemanticAnalyzer::CheckBase(LeafPtr Current)
{
	if (IsExpr(Current))
	{
		return CheckExpr(Current);
	}
	else if (IsConst(Current))
	{
		return CheckFactor(Current);
	}
	else if (Current->Type == LeafType::EXPR_CALL)
	{
		return CheckCall(Current);
	}
	return false;
}
void dbc::SemanticAnalyzer::EnterScope()
{
	ScopeStack->push_back(Scope());
}

void dbc::SemanticAnalyzer::ExitScope()
{
	if (ScopeStack->size() > 0)
	{
		ScopeStack->pop_back();
	}
	else
	{
		LeafPtr Current = nullptr;
		SemantError("Scope out of range");
	}
}
LeafPtr dbc::SemanticAnalyzer::FindSymbol(LeafPtr Symbol)
{
	for (Scope& CurrentScope : *ScopeStack)
	{
		for (LeafPtr Node : CurrentScope.SymbolTable)
		{
			if (Node->Type == LeafType::DECL_FUNC &&
				Symbol->Type == LeafType::EXPR_CALL)
			{
				if (strcmp(Node->Left->Left->STRING, Symbol->Left->STRING) == 0)
				{
					return Node;
				}
			}
			else if (Node->Type == LeafType::DECL_VAR &&
					 Symbol->Type == LeafType::CONST_IDENTIFIER)
			{
				if (strcmp(Node->Left->STRING, Symbol->STRING) == 0)
				{
					return Node;
				}
			}
			else if (Node->Type == LeafType::CONST_IDENTIFIER &&
					 Symbol->Type == LeafType::CONST_IDENTIFIER)
			{
				if (strcmp(Node->STRING, Symbol->STRING) == 0)
				{
					return Node;
				}
			}
		}
	}
	return nullptr;
}
void dbc::SemanticAnalyzer::AddSymbol(LeafPtr Current)
{
	if (ScopeStack->size() > 0)
	{
		if (Current->Type == LeafType::DECL_FUNC)
		{
			if (!SymbolExist(Current))
			{				
				ScopeStack->back().SymbolTable.push_back(Current);
			}
			else
			{
				SemantError("Multiple definition of function %s", Current->Left->Left->STRING);
			}
		}
		else
		{
			ScopeStack->back().SymbolTable.push_back(Current);
		}
	}
	else
	{
		SemantError("Invalid Scope.");
	}
}

bool dbc::SemanticAnalyzer::SymbolExist(LeafPtr Symbol)
{
	return FindSymbol(Symbol) != nullptr;
}
#pragma endregion
