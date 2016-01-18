#include <dbc/SyntaxAnalyzer.h>

using namespace dbc;

#if _DEBUG || DEBUG

#define NewLeaf(Type) new LeafNode(Type, CurrentToken != nullptr ? CurrentToken->Line : 0);					
#define DeleteToken(T) if (T) delete T;

#define TokenEquals(TestType) CurrentToken != nullptr && CurrentToken->Type == TestType
#define TokenNotEquals(TestType) CurrentToken != nullptr && CurrentToken->Type != TestType
#define TokenNextEquals(TestType) NextToken() != nullptr && NextToken()->Type == TestType
#define TokenNextNotEquals(TestType) NextToken() != nullptr && NextToken()->Type != TestType

#else

#define NewLeaf(Type) std::make_shared<LeafNode>(Type, CurrentToken != nullptr ? CurrentToken->Line : 0);					
#define DeleteToken(T) T.reset<dbc::Token>(nullptr);

#define TokenEquals(TestType) CurrentToken.get() != nullptr && CurrentToken->Type == TestType
#define TokenNotEquals(TestType) CurrentToken.get() != nullptr && CurrentToken->Type != TestType
#define TokenNextEquals(TestType) NextToken().get() != nullptr && NextToken()->Type == TestType
#define TokenNextNotEquals(TestType) NextToken().get() != nullptr && NextToken()->Type != TestType

#endif

#define ParseError(Message, ...) \
	fprintf(stderr, "\n%s ", SourcePath); \
	if (CurrentToken != nullptr) fprintf(stderr, "Line: %llu - Parsing Error: ", CurrentToken->Line);\
	fprintf(stderr, Message, ##__VA_ARGS__); \
	PAUSE; \
	exit(-1);



dbc::SyntaxAnalyzer::SyntaxAnalyzer(std::string & Source, const char * Path) :
	SourcePath(Path),
	Lexer(SourcePath),
	InFunction(false),
	InWhile(0),
	InIf(0)
{
	CurrentToken = nullptr;
	TokenStream = Lexer.Analyze(Source);
}

LeafPtr dbc::SyntaxAnalyzer::Analyze()
{
	PullToken();
	if (CurrentToken != nullptr)
	{
		return ParseProgram();
	}
	return NewLeaf(LeafType::ENDOFFILE);
}

void dbc::SyntaxAnalyzer::PullToken()
{
	if (TokenStream.size() > 0)
	{
		CurrentToken = TokenStream.front();
		TokenStream.pop_front();
	}
	else
	{
		DeleteToken(CurrentToken);
		CurrentToken = nullptr;
	}
}

TokenPtr dbc::SyntaxAnalyzer::NextToken(int64 Offset)
{
	if (TokenStream.size() > 0 && Offset < static_cast<int64>(TokenStream.size()))
	{
		return TokenStream[Offset];
	}
	return nullptr;
}

LeafPtr dbc::SyntaxAnalyzer::ParseProgram()
{
	return ParseStmtSeq();
}

LeafPtr dbc::SyntaxAnalyzer::ParseStmtSeq()
{
	LeafPtr Stmt = ParseStmt();
	if (CurrentToken != nullptr && Stmt != nullptr)
	{
		Stmt->Right = ParseStmtSeq();
	}
	return Stmt;
}

LeafPtr dbc::SyntaxAnalyzer::ParseStmt()
{
	LeafPtr Stmt = nullptr;
	if (CurrentToken != nullptr)
	{
		Stmt = NewLeaf(LeafType::STMT);
		if (TokenEquals(TokenType::WORD_IF))
		{
			if (!InFunction)
			{
				ParseError("Invalid if statement outside of function");
			}
			Stmt->Left = ParseStmtIf();
		}
		else if (TokenEquals(TokenType::IDENTIFIER) &&
				 TokenNextEquals(TokenType::SYM_EQU))
		{
			if (!InFunction)
			{
				ParseError("Invalid assign statement outside of function");
			}
			Stmt->Left = ParseStmtAssign();
		}
		else if (TokenEquals(TokenType::WORD_WHILE))
		{
			if (!InFunction)
			{
				ParseError("Invalid while statement outside of function");
			}
			Stmt->Left = ParseStmtWhile();
		}
		else if (TokenEquals(TokenType::WORD_LET))
		{
			Stmt->Left = ParseDeclVar();
		}
		else if (TokenEquals(TokenType::IDENTIFIER) &&
				 TokenNextEquals(TokenType::SYM_LPAREN))
		{
			if (!InFunction)
			{
				ParseError("Invalid call expression outside of function");
			}
			Stmt->Left = ParseExprCall();
		}
		else if (TokenEquals(TokenType::WORD_FUNC))
		{
			if (InFunction)
			{
				ParseError("Invalid function declaration inside other function");
			}
			InFunction = true;
			Stmt->Left = ParseDeclFunc();
			InFunction = false;
		}
		else if (TokenEquals(TokenType::WORD_RET))
		{
			if (!InFunction)
			{
				ParseError("Invalid return statement outside of function");
			}
			Stmt->Left = ParseStmtReturn();
		}
		else
		{
			Stmt->Left = ParseExprOr();
		}
		if (Stmt->Left == nullptr)
		{
			return nullptr;
		}
	}
	else
	{
		ParseError("Invalid Statement");
	}
	return Stmt;
}

LeafPtr dbc::SyntaxAnalyzer::ParseStmtIf()
{
	PullToken();
	LeafPtr IfStmt = NewLeaf(LeafType::STMT_IF);
	if (TokenNotEquals(TokenType::SYM_LPAREN))
	{
		ParseError("Missing ( in if statement");
	}
	PullToken();
	IfStmt->Left = ParseExprOr();
	if (TokenNotEquals(TokenType::SYM_RPAREN))
	{
		ParseError("Missing ) in if statement");
	}
	PullToken();
	IfStmt->Right = NewLeaf(LeafType::STMT);
	IfStmt->Right->Left = ParseStmtSeq();
	if (TokenEquals(TokenType::WORD_ELSEIF))
	{
		IfStmt->Right->Right = ParseStmtElseIf();
	}
	else if (TokenEquals(TokenType::WORD_ELSE))
	{
		IfStmt->Right->Right = ParseStmtElse();
	}
	else if (TokenNotEquals(TokenType::WORD_END))
	{
		ParseError("Missing 'END' in if statement");
	}
	else
	{
		PullToken();
	}
	return IfStmt;
}

LeafPtr dbc::SyntaxAnalyzer::ParseStmtElseIf()
{
	PullToken();
	LeafPtr ElseIfStmt = NewLeaf(LeafType::STMT_ELSEIF);
	if (TokenNotEquals(TokenType::SYM_LPAREN))
	{
		ParseError("Missing ( in elseif statement");
	}
	PullToken();
	ElseIfStmt->Left = ParseExprOr();
	if (TokenNotEquals(TokenType::SYM_RPAREN))
	{
		ParseError("Missing ) in elseif statement");
	}
	PullToken();
	ElseIfStmt->Right = NewLeaf(LeafType::STMT);
	ElseIfStmt->Right->Left = ParseStmtSeq();
	if (TokenEquals(TokenType::WORD_ELSEIF))
	{
		ElseIfStmt->Right->Right = ParseStmtElseIf();
	}
	else if (TokenEquals(TokenType::WORD_ELSE))
	{
		ElseIfStmt->Right->Right = ParseStmtElse();
	}
	else if (TokenNotEquals(TokenType::WORD_END))
	{
		ParseError("Missing 'END' in elseif statement");
	}
	else
	{
		PullToken();
	}
	return ElseIfStmt;
}

LeafPtr dbc::SyntaxAnalyzer::ParseStmtElse()
{
	PullToken();
	LeafPtr ElseStmt = NewLeaf(LeafType::STMT_ELSE);
	ElseStmt->Left = ParseStmtSeq();
	if (TokenNotEquals(TokenType::WORD_END))
	{
		ParseError("Missing 'END' in else statement");
	}
	PullToken();
	return ElseStmt;
}

LeafPtr dbc::SyntaxAnalyzer::ParseStmtAssign()
{
	LeafPtr Assign = NewLeaf(LeafType::STMT_ASSIGN);
	Assign->Left = ParseConstIdentifier();
	if (TokenNotEquals(TokenType::SYM_EQU))
	{
		ParseError("Missing = in assign statement");
	}
	PullToken();
	Assign->Right = ParseExprOr();
	if (Assign->Right == nullptr)
	{
		ParseError("Missing expression in assign statement");
	}
	return Assign;
}

LeafPtr dbc::SyntaxAnalyzer::ParseStmtWhile()
{
	PullToken();
	LeafPtr While = NewLeaf(LeafType::STMT_WHILE);
	if (TokenNotEquals(TokenType::SYM_LPAREN))
	{
		ParseError("Missing ( in while statement");
	}
	PullToken();
	While->Left = ParseExprOr();
	if (TokenNotEquals(TokenType::SYM_RPAREN))
	{
		ParseError("Missing ) in while statement");
	}
	PullToken();
	While->Right = ParseStmtSeq();
	if (TokenNotEquals(TokenType::WORD_END))
	{
		ParseError("Missing 'END' in while statement");
	}
	PullToken();
	return While;
}

LeafPtr dbc::SyntaxAnalyzer::ParseStmtReturn()
{
	PullToken();
	LeafPtr Ret = NewLeaf(LeafType::STMT_RETURN);
	Ret->Left = ParseExprOr();
	return Ret;
}

LeafPtr dbc::SyntaxAnalyzer::ParseDeclVar()
{
	PullToken();
	LeafPtr VarDecl = NewLeaf(LeafType::DECL_VAR);
	if (TokenNotEquals(TokenType::IDENTIFIER))
	{
		ParseError("Missing identifier in variable declaration")
	}
	VarDecl->Left = ParseConstIdentifier();
	if (TokenNotEquals(TokenType::SYM_EQU))
	{
		ParseError("Missing = in variable declaration");
	}
	PullToken();
	VarDecl->Right = ParseExprOr();
	if (VarDecl->Right == nullptr)
	{
		ParseError("Missing expression in variable declaration");
	}
	return VarDecl;
}

LeafPtr dbc::SyntaxAnalyzer::ParseDeclFunc()
{
	PullToken();
	LeafPtr FuncDecl = NewLeaf(LeafType::DECL_FUNC);
	if (TokenNotEquals(TokenType::IDENTIFIER))
	{
		ParseError("Missing identifier in function declaration")
	}
	FuncDecl->Left = NewLeaf(LeafType::DECL_HEAD);
	FuncDecl->Left->Left = ParseConstIdentifier();
	if (TokenNotEquals(TokenType::SYM_LPAREN))
	{
		ParseError("Missing ( in function declaration");
	}
	PullToken();
	FuncDecl->Left->Right = ParseDeclVarList();
	if (TokenNotEquals(TokenType::SYM_RPAREN))
	{
		ParseError("Missing ) in function declaration");
	}
	PullToken();
	FuncDecl->Right = ParseStmtSeq();
	if (TokenNotEquals(TokenType::WORD_END))
	{
		ParseError("Missing 'END' in function declaration");
	}
	PullToken();
	return FuncDecl;
}

LeafPtr dbc::SyntaxAnalyzer::ParseDeclVarList()
{
	if (TokenEquals(TokenType::SYM_RPAREN))
	{
		return nullptr;
	}
	LeafPtr VarList = NewLeaf(LeafType::DECL_VARLIST);
	VarList->Left = ParseConstIdentifier();
	if (VarList->Left == nullptr)
	{
		return nullptr;
	}
	if (TokenEquals(TokenType::SYM_COMMA))
	{
		PullToken();
		VarList->Right = ParseDeclVarList();
	}
	return VarList;
}

LeafPtr dbc::SyntaxAnalyzer::ParseExprList()
{
	LeafPtr Expr = NewLeaf(LeafType::EXPR_LIST);
	Expr->Left = ParseExprOr();
	if (Expr->Left == nullptr)
	{
		return nullptr;
	}
	if (TokenEquals(TokenType::SYM_COMMA))
	{
		PullToken();
		Expr->Right = ParseExprList();
	}
	return Expr;
}

LeafPtr dbc::SyntaxAnalyzer::ParseExprOr()
{
	LeafPtr Expr = nullptr;
	LeafPtr LHS = ParseExprAnd();
	if (TokenEquals(TokenType::WORD_OR))
	{
		Expr = NewLeaf(LeafType::EXPR_OR);
	}
	if (Expr != nullptr)
	{
		PullToken();
		Expr->Left = LHS;
		Expr->Right = ParseExprOr();
		if (Expr->Right == nullptr)
		{
			ParseError("Missing expression");
		}
		return Expr;
	}
	return LHS;
}

LeafPtr dbc::SyntaxAnalyzer::ParseExprAnd()
{
	LeafPtr Expr = nullptr;
	LeafPtr LHS = ParseExprNot();
	if (TokenEquals(TokenType::WORD_AND))
	{
		Expr = NewLeaf(LeafType::EXPR_AND);
	}
	if (Expr != nullptr)
	{
		PullToken();
		Expr->Left = LHS;
		Expr->Right = ParseExprAnd();
		if (Expr->Right == nullptr)
		{
			ParseError("Missing expression");
		}
		return Expr;
	}
	return LHS;
}

LeafPtr dbc::SyntaxAnalyzer::ParseExprNot()
{
	if (TokenEquals(TokenType::WORD_NOT))
	{
		PullToken();
		LeafPtr NotExpr = NewLeaf(LeafType::EXPR_NOT);
		NotExpr->Left = ParseExprCompare();
		if (NotExpr->Left == nullptr)
		{
			ParseError("Missing expression");
		}
		return NotExpr;
	}
	return ParseExprCompare();
}

LeafPtr dbc::SyntaxAnalyzer::ParseExprCompare()
{
	LeafPtr Expr = nullptr;
	LeafPtr LHS = ParseExprBitwise();
	if (TokenEquals(TokenType::SYM_EQU))
	{
		Expr = NewLeaf(LeafType::EXPR_CMPEQU);
	}
	else if (TokenEquals(TokenType::SYM_NOTEQU))
	{
		Expr = NewLeaf(LeafType::EXPR_CMPNOTEQU);
	}
	else if (TokenEquals(TokenType::SYM_GTEQU))
	{
		Expr = NewLeaf(LeafType::EXPR_CMPGTEQU);
	}
	else if (TokenEquals(TokenType::SYM_LTEQU))
	{
		Expr = NewLeaf(LeafType::EXPR_CMPLTEQU);
	}
	else if (TokenEquals(TokenType::SYM_LT))
	{
		Expr = NewLeaf(LeafType::EXPR_CMPLT);
	}
	else if (TokenEquals(TokenType::SYM_GT))
	{
		Expr = NewLeaf(LeafType::EXPR_CMPGT);
	}
	if (Expr != nullptr)
	{
		PullToken();
		Expr->Left = LHS;
		Expr->Right = ParseExprCompare();
		if (Expr->Right == nullptr)
		{
			ParseError("Missing expression");
		}
		return Expr;
	}
	return LHS;
}

LeafPtr dbc::SyntaxAnalyzer::ParseExprBitwise()
{
	LeafPtr Expr = nullptr;
	LeafPtr LHS = ParseExprMod();
	if (TokenEquals(TokenType::WORD_XOR))
	{
		Expr = NewLeaf(LeafType::EXPR_XOR);
	}
	else if (TokenEquals(TokenType::WORD_SHL))
	{
		Expr = NewLeaf(LeafType::EXPR_SHL);
	}
	else if (TokenEquals(TokenType::WORD_SHR))
	{
		Expr = NewLeaf(LeafType::EXPR_SHR);
	}
	else if (TokenEquals(TokenType::WORD_BOR))
	{
		Expr = NewLeaf(LeafType::EXPR_BOR);
	}
	else if (TokenEquals(TokenType::WORD_BAND))
	{
		Expr = NewLeaf(LeafType::EXPR_BAND);
	}
	if (Expr != nullptr)
	{
		PullToken();
		Expr->Left = LHS;
		Expr->Right = ParseExprBitwise();
		if (Expr->Right == nullptr)
		{
			ParseError("Missing expression");
		}
		return Expr;
	}
	return LHS;
}

LeafPtr dbc::SyntaxAnalyzer::ParseExprMod()
{
	LeafPtr Expr = nullptr;
	LeafPtr LHS = ParseExprAdd();
	if (TokenEquals(TokenType::WORD_MOD))
	{
		Expr = NewLeaf(LeafType::EXPR_MOD);
	}
	if (Expr != nullptr)
	{
		PullToken();
		Expr->Left = LHS;
		Expr->Right = ParseExprMod();
		if (Expr->Right == nullptr)
		{
			ParseError("Missing expression");
		}
		return Expr;
	}
	return LHS;
}

LeafPtr dbc::SyntaxAnalyzer::ParseExprAdd()
{
	LeafPtr Expr = nullptr;
	LeafPtr LHS = ParseExprMul();
	if (TokenEquals(TokenType::SYM_PLUS))
	{
		Expr = NewLeaf(LeafType::EXPR_ADD);
	}
	else if (TokenEquals(TokenType::SYM_MINUS))
	{
		Expr = NewLeaf(LeafType::EXPR_SUB);
	}
	if (Expr != nullptr)
	{
		PullToken();
		Expr->Left = LHS;
		Expr->Right = ParseExprAdd();
		if (Expr->Right == nullptr)
		{
			ParseError("Missing expression");
		}
		return Expr;
	}
	return LHS;
}

LeafPtr dbc::SyntaxAnalyzer::ParseExprMul()
{
	LeafPtr Expr = nullptr;
	LeafPtr LHS = ParseExprNegate();
	if (TokenEquals(TokenType::SYM_ASTERISK))
	{
		Expr = NewLeaf(LeafType::EXPR_MUL);
	}
	else if (TokenEquals(TokenType::SYM_SLASH))
	{
		Expr = NewLeaf(LeafType::EXPR_DIV);
	}
	if (Expr != nullptr)
	{
		PullToken();
		Expr->Left = LHS;
		Expr->Right = ParseExprMul();
		if (Expr->Right == nullptr)
		{
			ParseError("Missing expression");
		}
		return Expr;
	}
	return LHS;
}

LeafPtr dbc::SyntaxAnalyzer::ParseExprNegate()
{
	if (TokenEquals(TokenType::SYM_MINUS))
	{
		PullToken();
		LeafPtr Negate = NewLeaf(LeafType::EXPR_NEGATE);
		Negate->Left = ParseExprCall();
		if (Negate->Left == nullptr)
		{
			ParseError("Missing expression");
		}
		return Negate;
	}
	return ParseExprCall();
}

LeafPtr dbc::SyntaxAnalyzer::ParseExprCall()
{
	if (TokenEquals(TokenType::IDENTIFIER) &&
		TokenNextEquals(TokenType::SYM_LPAREN))
	{
		LeafPtr Call = NewLeaf(LeafType::EXPR_CALL);
		Call->Left = ParseConstIdentifier();
		if (TokenNotEquals(TokenType::SYM_LPAREN))
		{
			ParseError("Missing (");
		}
		PullToken();
		Call->Right = ParseExprList();
		if (TokenNotEquals(TokenType::SYM_RPAREN))
		{
			ParseError("Missing )");
		}
		PullToken();
		return Call;
	}
	return ParseFactor();
}

LeafPtr dbc::SyntaxAnalyzer::ParseFactor()
{
	LeafPtr Factor = nullptr;
	if (TokenEquals(TokenType::NUMBER))
	{
		Factor = ParseConstNumber();
	}
	else if (TokenEquals(TokenType::BOOLEAN))
	{
		Factor = ParseConstBoolean();
	}
	else if (TokenEquals(TokenType::IDENTIFIER))
	{
		Factor = ParseConstIdentifier();
	}
	else if (TokenEquals(TokenType::SYM_LPAREN))
	{
		PullToken();
		Factor = ParseExprOr();
		if (TokenNotEquals(TokenType::SYM_RPAREN))
		{
			ParseError("Missing ) at expression");
		}
		PullToken();
	}
	return Factor;
}

LeafPtr dbc::SyntaxAnalyzer::ParseConstNumber()
{
	LeafPtr NumConst = NewLeaf(LeafType::CONST_NUMBER);
	NumConst->UINT8 = static_cast<uint8>(std::stoi(CurrentToken->RawValue));
	PullToken();
	return NumConst;
}

LeafPtr dbc::SyntaxAnalyzer::ParseConstBoolean()
{
	LeafPtr BoolConst = NewLeaf(LeafType::CONST_BOOLEAN);
	BoolConst->BOOL = CurrentToken->RawValue == "1" ? true : false;
	PullToken();
	return BoolConst;
}

LeafPtr dbc::SyntaxAnalyzer::ParseConstIdentifier()
{
	LeafPtr IdentConst = NewLeaf(LeafType::CONST_IDENTIFIER);
	IdentConst->STRING = const_cast<char*>(CurrentToken->RawValue.c_str());
	PullToken();
	return IdentConst;
}
