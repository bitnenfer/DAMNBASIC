#include <dbc/Debug.h>

#define PRINTLN(m,...) printf(m"\n", ##__VA_ARGS__);

void dbc::PrintSyntaxTree(LeafPtr SyntaxTree, const char* Side, uint64 Deep)
{
	if (SyntaxTree != nullptr)
	{
		for (size_t index = 0; index < Deep; ++index)
		{
			printf("    ");
		}
		printf("%s ", Side);
		PrintLeaf(SyntaxTree);
		PrintSyntaxTree(SyntaxTree->Left, "L => ", Deep + 1);
		PrintSyntaxTree(SyntaxTree->Right, "R => ", Deep + 1);
	}
}

void dbc::PrintToken(dbc::TokenPtr Token)
{
	if (Token != nullptr)
	{
		TokenType Type = Token->Type;
		switch (Type)
		{
			case dbc::TokenType::SYM_LSQBRACK:
				PRINTLN("[");
				break;
			case dbc::TokenType::SYM_RSQBRACK:
				PRINTLN("]");
				break;
			case dbc::TokenType::LINEFEED:
				PRINTLN("line feed");
				break;
			case dbc::TokenType::WORD_MOD:
				PRINTLN("mod");
				break;
			case dbc::TokenType::WORD_SHR:
				PRINTLN("SHR");
				break;
			case dbc::TokenType::WORD_SHL:
				PRINTLN("shl");
				break;
			case dbc::TokenType::WORD_BOR:
				PRINTLN("bor");
				break;
			case dbc::TokenType::WORD_XOR:
				PRINTLN("xor");
				break;
			case dbc::TokenType::WORD_BAND:
				PRINTLN("band");
				break;
			case dbc::TokenType::WORD_AND:
				PRINTLN("and");
				break;
			case dbc::TokenType::WORD_OR:
				PRINTLN("or");
				break;
			case dbc::TokenType::ENDOFFILE:
				PRINTLN("eof");
				break;
			case dbc::TokenType::WORD_NOT:
				PRINTLN("not");
				break;
			case dbc::TokenType::WORD_FUNC:
				PRINTLN("func");
				break;
			case dbc::TokenType::WORD_IF:
				PRINTLN("if");
				break;
			case dbc::TokenType::WORD_ELSEIF:
				PRINTLN("elseif");
				break;
			case dbc::TokenType::WORD_ELSE:
				PRINTLN("else");
				break;
			case dbc::TokenType::WORD_LET:
				PRINTLN("let");
				break;
			case dbc::TokenType::WORD_END:
				PRINTLN("end");
				break;
			case dbc::TokenType::WORD_RET:
				PRINTLN("ret");
				break;
			case dbc::TokenType::SYM_NOTEQU:
				PRINTLN("<>");
				break;
			case dbc::TokenType::SYM_EQU:
				PRINTLN("=");
				break;
			case dbc::TokenType::SYM_LT:
				PRINTLN("<");
				break;
			case dbc::TokenType::SYM_GT:
				PRINTLN(">");
				break;
			case dbc::TokenType::SYM_LTEQU:
				PRINTLN("<=");
				break;
			case dbc::TokenType::SYM_GTEQU:
				PRINTLN(">=");
				break;
			case dbc::TokenType::SYM_PLUS:
				PRINTLN("+");
				break;
			case dbc::TokenType::SYM_MINUS:
				PRINTLN("-");
				break;
			case dbc::TokenType::SYM_SLASH:
				PRINTLN("/");
				break;
			case dbc::TokenType::SYM_ASTERISK:
				PRINTLN("*");
				break;
			case dbc::TokenType::SYM_COMMA:
				PRINTLN(",");
				break;
			case dbc::TokenType::SYM_LPAREN:
				PRINTLN(")");
				break;
			case dbc::TokenType::SYM_RPAREN:
				PRINTLN("(");
				break;
			case dbc::TokenType::IDENTIFIER:
				PRINTLN("identifier value = %s", Token->RawValue.c_str());
				break;
			case dbc::TokenType::NUMBER:
				PRINTLN("number value = %s", Token->RawValue.c_str());
				break;
			case dbc::TokenType::BOOLEAN:
				PRINTLN("boolean value = %s", Token->RawValue.c_str());
				break;
			default:
				PRINTLN("invalid");
				break;
		}
	}
	else
	{
		PRINTLN("Token is null");
	}
}

void dbc::PrintLeaf(LeafPtr Node)
{
	if (Node != nullptr)
	{
		LeafType Type = Node->Type;
		switch (Type)
		{
			case dbc::LeafType::STMT_MEMWRITE:
				PRINTLN("STMT_MEMWRITE");
				break;
			case dbc::LeafType::EXPR_MEMREAD:
				PRINTLN("EXPR_MEMREAD");
				break;
			case dbc::LeafType::CONST_MEMADDRESS:
				PRINTLN("EXPR_MEMADDRESS");
				break;
			case dbc::LeafType::EXPR_MOD:
				PRINTLN("EXPR_MOD");
				break;
			case dbc::LeafType::EXPR_SHR:
				PRINTLN("EXPR_SHR");
				break;
			case dbc::LeafType::EXPR_SHL:
				PRINTLN("EXPR_SHL");
				break;
			case dbc::LeafType::EXPR_BOR:
				PRINTLN("EXPR_BOR");
				break;
			case dbc::LeafType::EXPR_XOR:
				PRINTLN("EXPR_XOR");
				break;
			case dbc::LeafType::EXPR_BAND:
				PRINTLN("EXPR_BAND");
				break;
			case dbc::LeafType::PROGRAM:
				PRINTLN("PROGRAM");
				break;
			case dbc::LeafType::DECL_HEAD:
				PRINTLN("DECL_HEAD");
				break;
			case dbc::LeafType::STMT_SEQ:
				PRINTLN("STMT_SEQ");
				break;
			case dbc::LeafType::STMT:
				PRINTLN("STMT");
				break;
			case dbc::LeafType::STMT_IF:
				PRINTLN("STMT_IF");
				break;
			case dbc::LeafType::STMT_ELSEIF:
				PRINTLN("STMT_ELSEIF");
				break;
			case dbc::LeafType::STMT_ELSE:
				PRINTLN("STMT_ELSE");
				break;
			case dbc::LeafType::STMT_ASSIGN:
				PRINTLN("STMT_ASSIGN");
				break;
			case dbc::LeafType::STMT_WHILE:
				PRINTLN("STMT_WHILE");
				break;
			case dbc::LeafType::STMT_RETURN:
				PRINTLN("STMT_RETURN");
				break;
			case dbc::LeafType::DECL_VAR:
				PRINTLN("DECL_VAR");
				break;
			case dbc::LeafType::DECL_FUNC:
				PRINTLN("DECL_FUNC");
				break;
			case dbc::LeafType::DECL_VARLIST:
				PRINTLN("DECL_VARLIST");
				break;
			case dbc::LeafType::EXPR_LIST:
				PRINTLN("EXPR_LIST");
				break;
			case dbc::LeafType::EXPR_OR:
				PRINTLN("EXPR_OR");
				break;
			case dbc::LeafType::EXPR_AND:
				PRINTLN("EXPR_AND");
				break;
			case dbc::LeafType::EXPR_NOT:
				PRINTLN("EXPR_NOT");
				break;
			case dbc::LeafType::EXPR_CMPEQU:
				PRINTLN("EXPR_CMPEQU");
				break;
			case dbc::LeafType::EXPR_CMPGT:
				PRINTLN("EXPR_CMPGT");
				break;
			case dbc::LeafType::EXPR_CMPLT:
				PRINTLN("EXPR_CMPLT");
				break;
			case dbc::LeafType::EXPR_CMPNOTEQU:
				PRINTLN("EXPR_CMPNOTEQU");
				break;
			case dbc::LeafType::EXPR_CMPGTEQU:
				PRINTLN("EXPR_CMPGTEQU");
				break;
			case dbc::LeafType::EXPR_CMPLTEQU:
				PRINTLN("EXPR_CMPLTEQU");
				break;
			case dbc::LeafType::EXPR_ADD:
				PRINTLN("EXPR_ADD");
				break;
			case dbc::LeafType::EXPR_SUB:
				PRINTLN("EXPR_SUB");
				break;
			case dbc::LeafType::EXPR_DIV:
				PRINTLN("EXPR_DIV");
				break;
			case dbc::LeafType::EXPR_MUL:
				PRINTLN("EXPR_MUL");
				break;
			case dbc::LeafType::EXPR_NEGATE:
				PRINTLN("EXPR_NEGATE");
				break;
			case dbc::LeafType::EXPR_CALL:
				PRINTLN("EXPR_CALL");
				break;
			case dbc::LeafType::CONST_NUMBER:
				PRINTLN("CONST_NUMBER %u", Node->UINT16);
				break;
			case dbc::LeafType::CONST_BOOLEAN:
				PRINTLN("CONST_BOOLEAN value: %s", Node->BOOL ? "true" : "false");
				break;
			case dbc::LeafType::CONST_IDENTIFIER:
				PRINTLN("CONST_IDENTIFIER value: %s", Node->STRING);
				break;
			default:
				PRINTLN("INVALID");
				break;
		}
	}
	else
	{
		PRINTLN("Node is null");
	}
}
