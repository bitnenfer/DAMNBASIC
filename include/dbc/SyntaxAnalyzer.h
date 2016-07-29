#pragma once
#include <dbc/LexicalAnalyzer.h>
#include <dbc/LeafNode.h>

namespace dbc
{
class SyntaxAnalyzer
{
public:
	SyntaxAnalyzer(std::string& Source, const char* Path);
	LeafPtr Analyze();

protected:
	void PullToken();
	TokenPtr NextToken(int64 Offset = 0);
	// Statements
	LeafPtr ParseProgram();
	LeafPtr ParseStmtSeq();
	LeafPtr ParseStmt();
	LeafPtr ParseStmtIf();
	LeafPtr ParseStmtElseIf();
	LeafPtr ParseStmtElse();
	LeafPtr ParseStmtAssign();
	LeafPtr ParseMemWrite();
	LeafPtr ParseMemRead();
	LeafPtr ParseStmtWhile();
	LeafPtr ParseStmtReturn();
	// Declaration
	LeafPtr ParseDeclVar();
	LeafPtr ParseDeclFunc();
	LeafPtr ParseDeclVarList();
	LeafPtr ParseDeclNative();
	// Expressions
	LeafPtr ParseExprList();
	LeafPtr ParseExprOr();
	LeafPtr ParseExprAnd();
	LeafPtr ParseExprNot();
	LeafPtr ParseExprCompare();
	LeafPtr ParseExprBitwise();
	LeafPtr ParseExprMod();
	LeafPtr ParseExprAdd();
	LeafPtr ParseExprMul();
	LeafPtr ParseExprNegate();
	LeafPtr ParseExprCall();
	LeafPtr ParseFactor();
	LeafPtr ParseConstNumber();
	LeafPtr ParseConstBoolean();
	LeafPtr ParseConstIdentifier();

private:
	LexicalAnalyzer Lexer;
	std::deque<TokenPtr> TokenStream;
	TokenPtr CurrentToken;
	bool InFunction;
	int64 InWhile;
	int64 InIf;
	const char* SourcePath;
};
}