#pragma once
#include <dasm/Token.h>
#include <dasm/Node.h>
#include <dasm/Utility.h>
#include <vector>
#include <deque>

#define PRINT(LABEL, ...) printf(LABEL"\n", ##__VA_ARGS__);

namespace dasm
{
namespace Debug
{
static void PrintNode(NodePtr Node)
{
	ENode Type = Node->Type;
	switch (Type)
	{
		case dasm::ENode::ORIGIN:
			PRINT("ORIGIN");
			break;
		case dasm::ENode::LABEL:
			PRINT("LABEL VAL:%s", Node->STRING.c_str());
			break;
		case dasm::ENode::ARG_LIST:
			PRINT("ARG_LIST");
			break;
		case dasm::ENode::DECL_VAR:
			PRINT("DECL_VAR VAL:%s", Node->STRING.c_str());
			break;
		case dasm::ENode::ADDR_A:
			PRINT("ADDR_A");
			break;
		case dasm::ENode::ADDR_IMM:
			PRINT("ADDR_IMM");
			break;
		case dasm::ENode::ADDR_INDX:
			PRINT("ADDR_INDX");
			break;
		case dasm::ENode::ADDR_INDY:
			PRINT("ADDR_INDY");
			break;
		case dasm::ENode::ADDR_IND:
			PRINT("ADDR_IND");
			break;
		case dasm::ENode::ADDR_ZPY:
			PRINT("ADDR_ZPY");
			break;
		case dasm::ENode::ADDR_ZPX:
			PRINT("ADDR_ZPX");
			break;
		case dasm::ENode::ADDR_ABSY:
			PRINT("ADDR_ABSY");
			break;
		case dasm::ENode::ADDR_ABSX:
			PRINT("ADDR_ABSX");
			break;
		case dasm::ENode::ADDR_ZP:
			PRINT("ADDR_ZP");
			break;
		case dasm::ENode::ADDR_AB:
			PRINT("ADDR_AB");
			break;
		case dasm::ENode::ADDR_IMP:
			PRINT("ADDR_IMP");
			break;
		case dasm::ENode::INSTRUCTION:
			PRINT("INSTRUCTION OpCode: %s AddrMode: %s", OpCodeToString(Node->OpCode), AddrModeToString(Node->AddrMode));
			break;
		case dasm::ENode::LINE:
			PRINT("LINE");
			break;
		case dasm::ENode::ENDOFFILE:
			PRINT("ENDOFFILE");
			break;
		case dasm::ENode::DIR_WORD:
			PRINT("DIR_WORD");
			break;
		case dasm::ENode::DIR_BYTE:
			PRINT("DIR_BYTE");
			break;
		case dasm::ENode::DIR_INCLUDE:
			PRINT("DIR_INCLUDE");
			break;
		case dasm::ENode::HEX_ADDRESS:
			PRINT("HEX_ADDRESS VAL:%s", Node->STRING.c_str());
			break;
		case dasm::ENode::IDE_ADDRESS:
			PRINT("IDE_ADDRESS VAL:%s", Node->STRING.c_str());
			break;
		case dasm::ENode::HEX_VALUE:
			PRINT("HEX_VALUE VAL:%s", Node->STRING.c_str());
			break;
		case dasm::ENode::DEC_VALUE:
			PRINT("DEC_VALUE VAL:%d", Node->UINT8);
			break;
		case dasm::ENode::IDE_VALUE:
			PRINT("IDE_VALUE VAL:%s", Node->STRING.c_str());
			break;
		case dasm::ENode::STR_VALUE:
			PRINT("STR_VALUE VAL:%s", Node->STRING.c_str());
			break;
		default:
			PRINT("INVALID");
			break;
	}
}
void PrintAST(NodePtr Tree, const char* Side = "ROOT", uint64 Deep = 0)
{
	if (Tree != nullptr)
	{
		for (size_t index = 0; index < Deep; ++index)
		{
			printf("    ");
		}
		printf("%s ", Side);
		PrintNode(Tree);
		PrintAST(Tree->Left, "L => ", Deep + 1);
		PrintAST(Tree->Right, "R => ", Deep + 1);
	}
}
void PrintToken(TokenPtr Token)
{
	EToken Type = Token->Type;
	switch (Type)
	{
		case dasm::EToken::HEX_VALUE:
			PRINT("HEX_VAL\t\tVAL: %s", Token->RawValue.c_str());
			break;
		case dasm::EToken::HEX_ADDRESS:
			PRINT("HEX_ADD\t\tVAL: %s", Token->RawValue.c_str());
			break;
		case dasm::EToken::DEC_VALUE:
			PRINT("DEC_VAL\t\tVAL: %s", Token->RawValue.c_str());
			break;
		case dasm::EToken::IDENT_VALUE:
			PRINT("IDE_VAL\t\tVAL: %s", Token->RawValue.c_str());
			break;
		case dasm::EToken::SYM_A:
			PRINT("SYM_A");
			break;
		case dasm::EToken::SYM_X:
			PRINT("SYM_X");
			break;
		case dasm::EToken::SYM_Y:
			PRINT("SYM_Y");
			break;
		case dasm::EToken::NONE:
			PRINT("NONE");
			break;
		case dasm::EToken::MNEMONIC:
			PRINT("MNEMON\t\tVAL: %s", Token->RawValue.c_str());
			break;
		case dasm::EToken::IDENT_ADDRESS:
			PRINT("IDE_ADD\t\tVAL: %s", Token->RawValue.c_str());
			break;
		case dasm::EToken::SYM_PLUS:
			PRINT("SYM_PLUS");
			break;
		case dasm::EToken::SYM_MINUS:
			PRINT("SYM_MINUS");
			break;
		case dasm::EToken::SYM_ASTERISK:
			PRINT("SYM_ASTERISK");
			break;
		case dasm::EToken::SYM_SLASH:
			PRINT("SYM_SLASH");
			break;
		case dasm::EToken::SYM_EQUALS:
			PRINT("SYM_EQUALS");
			break;
		case dasm::EToken::SYM_LPAREN:
			PRINT("SYM_LPAREN");
			break;
		case dasm::EToken::SYM_RPAREN:
			PRINT("SYM_RPAREN");
			break;
		case dasm::EToken::SYM_PERCENT:
			PRINT("SYM_PERCENT");
			break;
		case dasm::EToken::SYM_LT:
			PRINT("SYM_LT");
			break;
		case dasm::EToken::SYM_GT:
			PRINT("SYM_GT");
			break;
		case dasm::EToken::SYM_SEMI:
			PRINT("SYM_SEMI");
			break;
		case dasm::EToken::SYM_COLON:
			PRINT("SYM_COLON");
			break;
		case dasm::EToken::SYM_HASH:
			PRINT("SYM_HASH");
			break;
		case dasm::EToken::SYM_DOLLAR:
			PRINT("SYM_DOLLAR");
			break;
		case dasm::EToken::SYM_COMMA:
			PRINT("SYM_COMMA");
			break;
		case dasm::EToken::WORD_BYTE:
			PRINT("WORD_BYTE");
			break;
		case dasm::EToken::WORD_WORD:
			PRINT("WORD_WORD");
			break;
		case dasm::EToken::WORD_INCLUDE:
			PRINT("WORD_INCLUDE");
			break;
		case dasm::EToken::STRING:
			PRINT("STRING\t\tVAL: %s", Token->RawValue.c_str());
			break;
		default:
			PRINT("INVALID");
			break;
	}
}
static void PrintTokenStream(std::deque<dasm::TokenPtr>& TokenStream)
{
	for (TokenPtr Tok : TokenStream)
	{
		PrintToken(Tok);
	}
}
static void PrintObjectCode(std::vector<uint8>& ObjectCode)
{
	int32 Count = 0;
	printf("\n");
	for (uint8 Code : ObjectCode)
	{
		printf("%s ", NumberToHex<uint8>(Code).c_str());
		if (++Count > 0xF)
		{
			Count = 0;
			printf("\n");
		}
	}
}
}
}