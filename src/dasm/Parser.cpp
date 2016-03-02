#include <dasm/Parser.h>
#include <dasm/OpCodes.h>
#include <dasm/Words.h>

using namespace dasm;

static bool IsHex(std::string& Test)
{
	for (auto c : Test)
	{
		int32 Index;
		bool GotMatch = false;
		for (Index = 0; Index < 21; ++Index)
		{
			if (c == WORD_HEXDIGIT[Index])
			{
				GotMatch = true;
			}
		}
		if (!GotMatch)
		{
			return false;
		}
	}
	return true;
}
static bool IsHexAlpha(std::string& Test)
{
	for (auto c : Test)
	{
		int32 Index;
		bool GotMatch = false;
		for (Index = 0; Index < 21; ++Index)
		{
			if (c == WORD_HEXALPHA[Index])
			{
				GotMatch = true;
			}
		}
		if (!GotMatch)
		{
			return false;
		}
	}
	return true;
}

#if _DEBUG || DEBUG

#define NewNode(Type) new Node(Type, CurrentToken != nullptr ? CurrentToken->Line : 0);					
#define DeleteToken(T) if (T) delete T;

#define TokenEquals(TestType) CurrentToken != nullptr && CurrentToken->Type == TestType
#define TokenNotEquals(TestType) CurrentToken != nullptr && CurrentToken->Type != TestType
#define TokenNextEquals(TestType) PeekToken() != nullptr && PeekToken()->Type == TestType
#define TokenNextNotEquals(TestType) PeekToken() != nullptr && PeekToken()->Type != TestType
#define TokenEqualsOffset(TestType, Offset) PeekToken(Offset) != nullptr && PeekToken(Offset)->Type != TestType

#else

#define NewNode(Type) std::make_shared<Node>(Type, CurrentToken != nullptr ? CurrentToken->Line : 0);					
#define DeleteToken(T) T.reset<dasm::Token>(nullptr);

#define TokenEquals(TestType) CurrentToken.get() != nullptr && CurrentToken->Type == TestType
#define TokenNotEquals(TestType) CurrentToken.get() != nullptr && CurrentToken->Type != TestType
#define TokenNextEquals(TestType) PeekToken().get() != nullptr && PeekToken()->Type == TestType
#define TokenNextNotEquals(TestType) PeekToken().get() != nullptr && PeekToken()->Type != TestType
#define TokenEqualsOffset(TestType, Offset) PeekToken(Offset).get() != nullptr && PeekToken(Offset)->Type != TestType

#endif

#define ParseError(Message, ...) \
	if (CurrentToken != nullptr) fprintf(stderr, "Line: %u - Parsing Error: ", CurrentToken->Line);\
	fprintf(stderr, Message, ##__VA_ARGS__); \
	PAUSE; \
	exit(-1);

NodePtr dasm::Parser::Parse(std::deque<TokenPtr>* TokenStream)
{
	this->TokenStream = TokenStream;
	PullToken();
	if (CurrentToken != nullptr)
	{
		return ParseProgram();
	}
	return NewNode(ENode::ENDOFFILE);
}

NodePtr dasm::Parser::ParseProgram()
{
	NodePtr Line = ParseLine();
	if (CurrentToken != nullptr && Line != nullptr)
	{
		Line->Right = ParseProgram();
	}
	return Line;
}

NodePtr dasm::Parser::ParseLine()
{
	NodePtr Line = NewNode(ENode::LINE);
	if (CurrentToken != nullptr)
	{
		if (TokenEquals(EToken::MNEMONIC))
		{
			Line->Left = ParseInstruction();
		}
		else if (TokenEquals(EToken::WORD_BYTE) ||
				 TokenEquals(EToken::WORD_WORD) ||
				 TokenEquals(EToken::WORD_INCLUDE))
		{
			Line->Left = ParseDirective();
		}
		else if (TokenEquals(EToken::IDENT_ADDRESS) &&
				 TokenNextEquals(EToken::SYM_EQUALS))
		{
			Line->Left = ParseDeclVar();
		}
		else if (TokenEquals(EToken::IDENT_ADDRESS)/* &&
				 TokenNextEquals(EToken::SYM_COLON)*/)
		{
			Line->Left = ParseLabel();
		}
		else if (TokenEquals(EToken::SYM_ASTERISK) &&
				 TokenNextEquals(EToken::SYM_EQUALS))
		{
			Line->Left = ParseOrigin();
		}
		else
		{
			ParseError("Invalid Line");
		}
	}
	return Line;
}

NodePtr dasm::Parser::ParseInstruction()
{
	NodePtr Instr = NewNode(ENode::INSTRUCTION);
	Instr->OpCode = GetOpCode(CurrentToken->RawValue);
	bool Branch = IsBranch(Instr->OpCode);
	if (IsImplicit(Instr->OpCode))
	{
		Instr->AddrMode = EAddressMode::IMP;
		PullToken();
	}
	else
	{
		TokenPtr LastToken = CurrentToken;
		PullToken();
		NodePtr Addr = ParseAddressMode();
		if (Addr == nullptr)
		{
			CurrentToken = LastToken;
			ParseError("Invalid Address");
		}
		if (Addr->Type == ENode::ADDR_A)
		{
			Instr->AddrMode = EAddressMode::A;
		}
		else if (Addr->Type == ENode::ADDR_AB)
		{
			Instr->AddrMode = EAddressMode::AB;
			Instr->Left = Addr->Left;
		}
		else if (Addr->Type == ENode::ADDR_ABSX)
		{
			Instr->AddrMode = EAddressMode::ABX;
			Instr->Left = Addr->Left;
		}
		else if (Addr->Type == ENode::ADDR_ABSY)
		{
			Instr->AddrMode = EAddressMode::ABY;
			Instr->Left = Addr->Left;
		}
		else if (Addr->Type == ENode::ADDR_ZP)
		{
			Instr->AddrMode = EAddressMode::ZP;
			Instr->Left = Addr->Left;
		}
		else if (Addr->Type == ENode::ADDR_ZPX)
		{
			Instr->AddrMode = EAddressMode::ZPX;
			Instr->Left = Addr->Left;
		}
		else if (Addr->Type == ENode::ADDR_ZPY)
		{
			Instr->AddrMode = EAddressMode::ZPY;
			Instr->Left = Addr->Left;
		}
		else if (Addr->Type == ENode::ADDR_IND)
		{
			Instr->AddrMode = EAddressMode::IND;
			Instr->Left = Addr->Left;
		}
		else if (Addr->Type == ENode::ADDR_INDX)
		{
			Instr->AddrMode = EAddressMode::IDX;
			Instr->Left = Addr->Left;
		}
		else if (Addr->Type == ENode::ADDR_INDY)
		{
			Instr->AddrMode = EAddressMode::IDY;
			Instr->Left = Addr->Left;
		}
		else if (Addr->Type == ENode::ADDR_IMM)
		{
			Instr->AddrMode = EAddressMode::IMM;
			Instr->Left = Addr->Left;
		}
		if (Branch)
		{
			Instr->AddrMode = EAddressMode::REL;
		}

	}
	return Instr;
}

NodePtr dasm::Parser::ParseOrigin()
{
	NodePtr Origin = NewNode(ENode::ORIGIN);
	PullToken();
	PullToken();
	Origin->Left = ParseHexAddress();
	return Origin;
}

// Accumulator		OPC A
// Immediate		OPC #$A5       
// Zero Page		OPC $A5        
// Zero Page,X		OPC $A5,X 
// Zero Page,Y		OPC $A5,Y
// Absolute			OPC $A5B6      
// Absolute,X		OPC $A5B6,X 
// Absolute,Y		OPC $A5B6,Y 
// Indirect			OPC ($A5B6)
// (Indirect,X)		OPC ($A5,X)    
// (Indirect),Y		OPC ($A5),Y
NodePtr dasm::Parser::ParseAddressMode()
{
	NodePtr AddressMode = nullptr;
	// Accumulator
	if (TokenEquals(EToken::SYM_A))
	{
		AddressMode = NewNode(ENode::ADDR_A);
		PullToken();
		return AddressMode;
	}
	// Immediate
	else if (TokenEquals(EToken::HEX_VALUE) ||
		TokenEquals(EToken::DEC_VALUE) ||
		TokenEquals(EToken::IDENT_VALUE))
	{
		AddressMode = NewNode(ENode::ADDR_IMM);
		AddressMode->Left = ParseValue();
		return AddressMode;
	}
	// Zero Page
	else if ((TokenEquals(EToken::HEX_ADDRESS) &&
			 CurrentToken->RawValue.length() <= 2))
	{
		if (TokenNextEquals(EToken::SYM_COMMA) &&
			TokenEqualsOffset(EToken::SYM_X, 1))
		{
			AddressMode = NewNode(ENode::ADDR_ZPX);
			AddressMode->Left = ParseAddress();
			PullToken();
			PullToken();
			return AddressMode;
		}
		else if (TokenNextEquals(EToken::SYM_COMMA) &&
				 TokenEqualsOffset(EToken::SYM_Y, 1))
		{
			AddressMode = NewNode(ENode::ADDR_ZPY);
			AddressMode->Left = ParseAddress();
			PullToken();
			PullToken();
			return AddressMode;
		}
		else
		{
			AddressMode = NewNode(ENode::ADDR_ZP);
			AddressMode->Left = ParseAddress();
			PullToken();
			return AddressMode;
		}
	}
	// Absolute
	else if ((TokenEquals(EToken::HEX_ADDRESS) &&
			 CurrentToken->RawValue.length() > 2) ||
			 TokenEquals(EToken::IDENT_ADDRESS))
	{
		if (TokenNextEquals(EToken::SYM_COMMA) &&
			TokenEqualsOffset(EToken::SYM_X, 1))
		{
			AddressMode = NewNode(ENode::ADDR_ABSX);
			AddressMode->Left = ParseAddress();
			PullToken();
			PullToken();
			return AddressMode;
		}
		else if (TokenNextEquals(EToken::SYM_COMMA) &&
				 TokenEqualsOffset(EToken::SYM_Y, 1))
		{
			AddressMode = NewNode(ENode::ADDR_ABSY);
			AddressMode->Left = ParseAddress();
			PullToken();
			PullToken();
			return AddressMode;
		}
		else
		{
			AddressMode = NewNode(ENode::ADDR_AB);
			AddressMode->Left = ParseAddress();
			return AddressMode;
		}
	}
	// Indirect
	else if (TokenEquals(EToken::SYM_LPAREN) &&
			 (TokenNextEquals(EToken::HEX_ADDRESS) ||
			 TokenNextEquals(EToken::IDENT_ADDRESS)))
	{
		PullToken();
		NodePtr Value = ParseAddress();
		if (TokenEquals(EToken::SYM_COMMA) &&
			TokenNextEquals(EToken::SYM_X))
		{
			AddressMode = NewNode(ENode::ADDR_INDX);
			PullToken();
			PullToken();
			if (TokenNotEquals(EToken::SYM_RPAREN))
			{
				ParseError("Missing ) in Indirect X address mode");
			}
		}
		else if (TokenEquals(EToken::SYM_RPAREN) &&
				 TokenNextEquals(EToken::SYM_COMMA) &&
				 TokenEqualsOffset(EToken::SYM_Y, 1))
		{
			AddressMode = NewNode(ENode::ADDR_INDX);
			PullToken();
			PullToken();
			PullToken();
		}
		else
		{
			ParseError("Invalid Indirect address mode.");
		}
		AddressMode->Left = Value;
		return AddressMode;
	}
	else
	{
		return nullptr;
		//ParseError("Invalid address mode.");
	}
	return AddressMode;
}

NodePtr dasm::Parser::ParseByteDirective()
{
	NodePtr ByteDir = NewNode(ENode::DIR_BYTE);
	PullToken();
	NodePtr Args = NewNode(ENode::ARG_LIST);
	NodePtr Head = Args;
	while (TokenEquals(EToken::HEX_ADDRESS) ||
		   TokenEquals(EToken::STRING))
	{
		if (TokenEquals(EToken::HEX_ADDRESS))
		{
			Args->Left = ParseHexAddress();
			if (!Args->Left->Is8Bit)
			{
				ParseError("BYTE Directive can only have 8 bit valus as arguments.");
			}
		}
		else if (TokenEquals(EToken::STRING))
		{
			Args->Left = ParseString();
		}
		else
		{
			ParseError("Invalid value list for BYTE directive.");
		}
		if (TokenEquals(EToken::SYM_COMMA))
		{
			PullToken();
			Args->Right = NewNode(ENode::ARG_LIST);
			Args = Args->Right;
		}
		else
		{
			break;
		}
	}
	ByteDir->Left = Head;
	return ByteDir;
}

NodePtr dasm::Parser::ParseWordDirective()
{
	NodePtr WordDir = NewNode(ENode::DIR_WORD);
	PullToken();
	NodePtr Args = NewNode(ENode::ARG_LIST);
	NodePtr Head = Args;
	while (TokenEquals(EToken::HEX_ADDRESS) ||
		   TokenEquals(EToken::STRING))
	{
		if (TokenEquals(EToken::HEX_ADDRESS))
		{
			Args->Left = ParseHexAddress();
			if (Args->Left->Is8Bit)
			{
				ParseError("WORD Directive can only have 16 bit valus as arguments.");
			}
		}
		else
		{
			ParseError("Invalid value list for WORD directive.");
		}
		if (TokenEquals(EToken::SYM_COMMA))
		{
			PullToken();
			Args->Right = NewNode(ENode::ARG_LIST);
			Args = Args->Right;
		}
		else
		{
			break;
		}
	}
	WordDir->Left = Head;
	return WordDir;
}

NodePtr dasm::Parser::ParseIncludeDirective()
{
	NodePtr Include = NewNode(ENode::DIR_INCLUDE);
	PullToken();
	Include->Left = ParseString();
	return Include;
}

NodePtr dasm::Parser::ParseAddress()
{
	if (TokenEquals(EToken::HEX_ADDRESS))
	{
		return ParseHexAddress();
	}
	else if (TokenEquals(EToken::IDENT_ADDRESS))
	{
		return ParseIdentAddress();
	}
	else
	{
		ParseError("Invalid Address");
	}
	return nullptr;
}

NodePtr dasm::Parser::ParseValue()
{
	if (TokenEquals(EToken::DEC_VALUE))
	{
		return ParseDecValue();
	}
	else if (TokenEquals(EToken::HEX_VALUE))
	{
		return ParseHexValue();
	}
	else if (TokenEquals(EToken::IDENT_VALUE))
	{
		return ParseIdentValue();
	}
	else
	{
		ParseError("Invalid Value");
	}
	return nullptr;
}

NodePtr dasm::Parser::ParseLabel()
{
	NodePtr Label = NewNode(ENode::LABEL);
	Label->Left = ParseIdentAddress();
	return Label;
}

NodePtr dasm::Parser::ParseDeclVar()
{
	NodePtr DeclVar = NewNode(ENode::DECL_VAR);
	DeclVar->Left = ParseIdentAddress();
	PullToken();
	DeclVar->Right = ParseHexAddress();
	return DeclVar;
}

NodePtr dasm::Parser::ParseDirective()
{
	NodePtr Directive = nullptr;
	if (TokenEquals(EToken::WORD_BYTE))
	{
		Directive = ParseByteDirective();
	}
	else if (TokenEquals(EToken::WORD_WORD))
	{
		Directive = ParseWordDirective();
	}
	else if (TokenEquals(EToken::WORD_INCLUDE))
	{
		Directive = ParseIncludeDirective();
	}
	return Directive;
}

NodePtr dasm::Parser::ParseHexAddress()
{
	NodePtr HexAddr = NewNode(ENode::HEX_VALUE);
	HexAddr->STRING = CurrentToken->RawValue;
	HexAddr->IsHex = true;
	HexAddr->Is8Bit = CurrentToken->RawValue.length() <= 2;
	PullToken();
	return HexAddr;
}

NodePtr dasm::Parser::ParseHexValue()
{
	NodePtr HexValue = NewNode(ENode::HEX_VALUE);
	HexValue->STRING = CurrentToken->RawValue;
	HexValue->Is8Bit = CurrentToken->RawValue.length() <= 2;
	HexValue->IsHex = true;
	PullToken();
	return HexValue;
}

NodePtr dasm::Parser::ParseIdentAddress()
{
	NodePtr IdentAddr = NewNode(ENode::IDE_ADDRESS);
	IdentAddr->STRING = CurrentToken->RawValue;
	PullToken();
	return IdentAddr;
}

NodePtr dasm::Parser::ParseIdentValue()
{
	NodePtr IdentValue = NewNode(ENode::IDE_VALUE);
	IdentValue->STRING = CurrentToken->RawValue;
	PullToken();
	return IdentValue;
}

NodePtr dasm::Parser::ParseDecValue()
{
	NodePtr DecValue = NewNode(ENode::DEC_VALUE);
	uint16 Value = static_cast<uint16>(std::atoi(CurrentToken->RawValue.c_str()));
	if (Value <= 0xFF)
	{
		DecValue->UINT8 = static_cast<uint8>(Value);
		DecValue->Is8Bit = true;
	}
	else
	{
		DecValue->UINT16 = Value;
		DecValue->Is8Bit = false;
	}
	PullToken();
	return DecValue;
}

NodePtr dasm::Parser::ParseString()
{
	if (TokenEquals(EToken::STRING))
	{
		NodePtr String = NewNode(ENode::STR_VALUE);
		String->STRING = CurrentToken->RawValue;
		PullToken();
		return String;
	}
	else
	{
		ParseError("Invalid String");
	}
	return nullptr;
}

void dasm::Parser::PullToken()
{
	if (TokenStream->size() > 0)
	{
		CurrentToken = TokenStream->front();
		TokenStream->pop_front();
	}
	else
	{
		DeleteToken(CurrentToken);
		CurrentToken = nullptr;
	}
}

TokenPtr dasm::Parser::PeekToken(int32 Offset)
{
	if (TokenStream->size() > 0 && Offset < static_cast<int32>(TokenStream->size()))
	{
		return (*TokenStream)[Offset];
	}
	return nullptr;
}
