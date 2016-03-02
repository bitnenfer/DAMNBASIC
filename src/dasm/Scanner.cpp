#include <dasm/Scanner.h>
#include <cctype>
#include <dasm/OpCodes.h>

using namespace dasm;

#if _DEBUG || DEBUG
#define NewToken(Type) new dasm::Token(Type, CurrentLine)
#define DeleteToken(Token) 
#else
#define NewToken(Type) std::make_shared<dasm::Token>(Type, CurrentLine)
#define DeleteToken(T) T.reset<Token>(nullptr)
#endif

#define IsWhitespace(n) n == ' ' || n == '\n' || n == '\r' || n == '\t' || n == 0

#define ScannerError(Message, ...) \
		fprintf(stderr, "Line %u Scanning Error: ", CurrentLine); \
		fprintf(stderr, Message, ##__VA_ARGS__); \
		fprintf(stderr, "\n"); \
		PAUSE; \
		exit(-1);

void dasm::Scanner::Scan(const std::string* Source)
{
	Data = Source;
	Length =static_cast<int32>(Data->length());
	Index = 0;
	CurrentLine = 1;
	while (!Eof() || Current == 0)
	{
		TokenPtr Token = nullptr;
		GetNext();
		if (Current == 0)
		{
			break; 
		}
		else if (ScanComment() || ScanWhiteSpace()) continue;
		else if (ScanSymbol(Token));
		else if (ScanKeyword(Token));
		else if (ScanTerminal(Token));
		else
		{
			ScannerError("Invalid symbol \"%c\" [ASCII=%d]", Current, Current);
			break;
		}
		if (Token != nullptr)
		{
			TokenStream.push_back(Token);
		}
	}
}

std::deque<TokenPtr>& dasm::Scanner::GetTokenStream()
{
	return TokenStream;
}

bool dasm::Scanner::ScanWhiteSpace()
{
	if (Current == SYMBOL_LF)
	{
		++CurrentLine;
		return true;
	}
	return (Current == SYMBOL_SPACE ||
			Current == SYMBOL_CR ||
			Current == SYMBOL_TB);
}

bool dasm::Scanner::ScanComment()
{
	if (Current == SYMBOL_SEMICOLON)
	{
		while (Current != SYMBOL_LF &&
			   Current != SYMBOL_CR)
		{
			GetNext();
		}
		++CurrentLine;
		return true;
	}
	return false;
}

bool dasm::Scanner::ScanKeyword(TokenPtr & Token)
{
	if (ScanByte(Token)) return true;
	if (ScanWord(Token)) return true;
	if (ScanInclude(Token)) return true;
	return false;
}

bool dasm::Scanner::ScanIdentifier(TokenPtr & Token)
{
	if (ScanCharInString(Current, WORD_ALPHA))
	{
		int32 LastIndex = Index;
		Token = NewToken(EToken::IDENT_ADDRESS);
		Token->RawValue += Current;
		GetNext();
		while (ScanCharInString(Current, WORD_ALPHA) ||
			   ScanCharInString(Current, WORD_DIGIT))
		{
			Token->RawValue += Current;
			GetNext();
		}
		DecIndex();
		if (GetOpCode(Token->RawValue) != EOpCode::NOOP ||
			Token->RawValue == "A" || Token->RawValue == "X" ||
			Token->RawValue == "Y" || Token->RawValue == "a" ||
			Token->RawValue == "x" || Token->RawValue == "y")
		{
			Index = LastIndex - 1;
			return true;
		}
		return true;
	}
	return false;
}

bool dasm::Scanner::ScanSymbol(TokenPtr & Token)
{
	if (ScanMnemonic(Token)) return true;
	if (ScanNumber(Token)) return true;
	if (ScanPlus(Token)) return true;
	if (ScanMinus(Token)) return true;
	if (ScanAsterisk(Token)) return true;
	if (ScanSlash(Token)) return true;
	if (ScanEquals(Token)) return true;
	if (ScanLParen(Token)) return true;
	if (ScanRParen(Token)) return true;
	if (ScanPercent(Token)) return true;
	if (ScanLT(Token)) return true;
	if (ScanGT(Token)) return true;
	if (ScanSemi(Token)) return true;
	if (ScanColon(Token)) return true;
	if (ScanHash(Token)) return true;
	if (ScanComma(Token)) return true;
	if (ScanDollar(Token)) return true;
	return false;
}

bool dasm::Scanner::ScanTerminal(TokenPtr & Token)
{
	if (ScanA(Token)) return true;
	if (ScanX(Token)) return true;
	if (ScanY(Token)) return true;
	if (ScanString(Token)) return true;
	if (ScanIdentifier(Token)) return true;
	return false;
}

bool dasm::Scanner::ScanKeywordName(const char * Keyword)
{
	if (toupper(Current) == Keyword[0])
	{
		int32 Index;
		int32 Length = static_cast<int32>(strlen(Keyword));
		for (Index = 0; Index < Length; ++Index)
		{
			if (toupper(Peek(Index - 1)) != Keyword[Index])
			{
				return false;
			}
		}
		for (Index = 0; Index < Length; ++Index)
		{
			GetNext();
		}
		DecIndex();
		return true;
	}
	return false;
}

bool dasm::Scanner::ScanCharInString(char Value, const char * Test)
{
	int64 Length = strlen(Test);
	for (int64 Index = 0; Index < Length; ++Index)
	{
		if (Test[Index] == Value)
		{
			return true;
		}
	}
	return false;
}

bool dasm::Scanner::ScanNumber(TokenPtr & Token)
{
	if (Current == '#' &&
		Peek() == '$' &&
		ScanCharInString(Peek(1), WORD_HEXDIGIT))
	{
		Token = NewToken(EToken::HEX_VALUE);
		GetNext();
		GetNext();
		while (ScanCharInString(Current, WORD_HEXDIGIT))
		{
			Token->RawValue += Current;
			GetNext();
		}
		DecIndex();
		return true;
	}
	else if (Current == '$' &&
			 ScanCharInString(Peek(), WORD_HEXDIGIT))
	{
		Token = NewToken(EToken::HEX_ADDRESS);
		GetNext();
		while (ScanCharInString(Current, WORD_HEXDIGIT))
		{
			Token->RawValue += Current;
			GetNext();
		}
		DecIndex();
		return true;
	}
	else if (Current == '#' &&
			 ScanCharInString(Peek(), WORD_DIGIT))
	{
		Token = NewToken(EToken::DEC_VALUE);
		GetNext();
		while (ScanCharInString(Current, WORD_DIGIT))
		{
			Token->RawValue += Current;
			GetNext();
			if (ScanCharInString(Current, WORD_HEXALPHA))
			{
				return false;
			}
		}
		DecIndex();
		return true;
	}
	else if (Current == '#' &&
			 ScanCharInString(Peek(), WORD_ALPHA))
	{
		Token = NewToken(EToken::IDENT_VALUE);
		GetNext();
		if (ScanCharInString(Current, WORD_ALPHA))
		{
			Token->RawValue += Current;
			GetNext();
			while (ScanCharInString(Current, WORD_ALPHA) ||
				   ScanCharInString(Current, WORD_DIGIT))
			{
				Token->RawValue += Current;
				GetNext();
			}
			DecIndex();
			return true;
		}
	}
	return false;
}

bool dasm::Scanner::ScanString(TokenPtr & Token)
{
	if (Current == '"')
	{
		GetNext();
		Token = NewToken(EToken::STRING);
		while (Current != '"' && !Eof())
		{
			Token->RawValue += Current;
			GetNext();
		}
		return true;
	}
	return false;
}

bool dasm::Scanner::ScanPlus(TokenPtr & Token)
{
	if (Current == SYMBOL_PLUS)
	{
		Token = NewToken(EToken::SYM_PLUS);
		return true;
	}
	return false;
}

bool dasm::Scanner::ScanMinus(TokenPtr & Token)
{
	if (Current == SYMBOL_MINUS)
	{
		Token = NewToken(EToken::SYM_MINUS);
		return true;
	}
	return false;
}

bool dasm::Scanner::ScanAsterisk(TokenPtr & Token)
{
	if (Current == SYMBOL_ASTERISK)
	{
		Token = NewToken(EToken::SYM_ASTERISK);
		return true;
	}
	return false;
}

bool dasm::Scanner::ScanSlash(TokenPtr & Token)
{
	if (Current == SYMBOL_SLASH)
	{
		Token = NewToken(EToken::SYM_SLASH);
		return true;
	}
	return false;
}

bool dasm::Scanner::ScanEquals(TokenPtr & Token)
{
	if (Current == SYMBOL_EQUAL)
	{
		Token = NewToken(EToken::SYM_EQUALS);
		return true;
	}
	return false;
}

bool dasm::Scanner::ScanLParen(TokenPtr & Token)
{
	if (Current == SYMBOL_LPAREN)
	{
		Token = NewToken(EToken::SYM_LPAREN);
		return true;
	}
	return false;
}

bool dasm::Scanner::ScanRParen(TokenPtr & Token)
{
	if (Current == SYMBOL_RPAREN)
	{
		Token = NewToken(EToken::SYM_RPAREN);
		return true;
	}
	return false;
}

bool dasm::Scanner::ScanPercent(TokenPtr & Token)
{
	if (Current == SYMBOL_PERCENT)
	{
		Token = NewToken(EToken::SYM_PERCENT);
		return true;
	}
	return false;
}

bool dasm::Scanner::ScanLT(TokenPtr & Token)
{
	if (Current == SYMBOL_LT)
	{
		Token = NewToken(EToken::SYM_LT);
		return true;
	}
	return false;
}

bool dasm::Scanner::ScanGT(TokenPtr & Token)
{
	if (Current == SYMBOL_GT)
	{
		Token = NewToken(EToken::SYM_GT);
		return true;
	}
	return false;
}

bool dasm::Scanner::ScanSemi(TokenPtr & Token)
{
	if (Current == SYMBOL_SEMICOLON)
	{
		Token = NewToken(EToken::SYM_SEMI);
		return true;
	}
	return false;
}

bool dasm::Scanner::ScanColon(TokenPtr & Token)
{
	if (Current == SYMBOL_COLON)
	{
		Token = NewToken(EToken::SYM_COLON);
		return true;
	}
	return false;
}

bool dasm::Scanner::ScanHash(TokenPtr & Token)
{
	if (Current == SYMBOL_HASH)
	{
		Token = NewToken(EToken::SYM_HASH);
		return true;
	}
	return false;
}

bool dasm::Scanner::ScanDollar(TokenPtr & Token)
{
	if (Current == SYMBOL_DOLLAR)
	{
		Token = NewToken(EToken::SYM_DOLLAR);
		return true;
	}
	return false;
}

bool dasm::Scanner::ScanComma(TokenPtr & Token)
{
	if (Current == SYMBOL_COMMA)
	{
		Token = NewToken(EToken::SYM_COMMA);
		return true;
	}
	return false;
}

bool dasm::Scanner::ScanA(TokenPtr & Token)
{
	if ((Current == 'A' || Current == 'a') && IsWhitespace(Peek()))
	{
		Token = NewToken(EToken::SYM_A);
		return true;
	}
	return false;
}

bool dasm::Scanner::ScanX(TokenPtr & Token)
{
	if ((Current == 'X' || Current == 'x') && IsWhitespace(Peek()))
	{
		Token = NewToken(EToken::SYM_X);
		return true;
	}
	return false;
}

bool dasm::Scanner::ScanY(TokenPtr & Token)
{
	if ((Current == 'Y' || Current == 'y') && IsWhitespace(Peek()))
	{
		Token = NewToken(EToken::SYM_Y);
		return true;
	}
	return false;
}

bool dasm::Scanner::ScanByte(TokenPtr & Token)
{
	if (ScanKeywordName(KEYWORD_BYTE))
	{
		Token = NewToken(EToken::WORD_BYTE);
		return true;
	}
	return false;
}

bool dasm::Scanner::ScanWord(TokenPtr & Token)
{
	if (ScanKeywordName(KEYWORD_WORD))
	{
		Token = NewToken(EToken::WORD_WORD);
		return true;
	}
	return false;
}

bool dasm::Scanner::ScanInclude(TokenPtr & Token)
{
	if (ScanKeywordName(KEYWORD_INCLUDE))
	{
		Token = NewToken(EToken::WORD_INCLUDE);
		return true;
	}
	return false;
}

bool dasm::Scanner::ScanMnemonic(TokenPtr & Token)
{
	auto Idx = 0;
	for (Idx = 0; Idx < MNEMONICS_COUNT; ++Idx)
	{
		auto Mnemonic = KEYWORD_MNEMONICS[Idx];
		if (ScanKeywordName(Mnemonic))
		{
			Token = NewToken(EToken::MNEMONIC);
			Token->RawValue += Mnemonic;
			return true;
		}
	}
	return false;
}

bool dasm::Scanner::Eof(int32 Step)
{
	return Index + Step >= Length;
}

char dasm::Scanner::Peek(int32 Step)
{
	if (!Eof(Step))
	{
		return (*Data)[Index + Step];
	}
	return 0;
}

char dasm::Scanner::GetNext()
{
	return (Current = (*Data)[Index++]);
}
