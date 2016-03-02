#include <dbc/LexicalAnalyzer.h>

using namespace dbc;

#if _DEBUG || DEBUG
#define NewToken(Type) new dbc::Token(Type, LineCount);
#define DeleteToken(T) 
#else
#define NewToken(Type) std::make_shared<dbc::Token>(Type, LineCount);
#define DeleteToken(T) T.reset<dbc::Token>(nullptr);
#endif

#define LexerError(Message, ...) \
		fprintf(stderr, "\n%s ", SourcePath); \
		fprintf(stderr, "Line %zu Lexing Error: ", LineCount); \
		fprintf(stderr, Message, ##__VA_ARGS__); \
		fprintf(stderr, "\n"); \
		PAUSE; \
		exit(-1);

dbc::LexicalAnalyzer::LexicalAnalyzer(const char * File) :
	Index(0),
	Length(0),
	LineCount(1),
	LastChar(-1),
	SourcePath(File)
{}

dbc::LexicalAnalyzer::~LexicalAnalyzer()
{
	ResetTokenStream();
}

std::deque<TokenPtr>& dbc::LexicalAnalyzer::Analyze(const std::string & _Source)
{
	ResetTokenStream();
	Source = _Source;
	Length = Source.length();
	while (!Eof())
	{
		TokenPtr Token = nullptr;
		LastChar = GetNextChar();
		if (LastChar == -1) break;
		else if (ScanEmptySpace()) continue;
		else if (ScanLinefeed(Token));
		else if (ScanSymbols(Token));
		else if (ScanKeywords(Token));
		else if (ScanTerminalSymbols(Token));
		else
		{
			LexerError("Invalid symbol \"%c\" [ASCII=%d]", LastChar, LastChar);
			break;
		}
		if (Token != nullptr)
		{
			TokenStream.push_back(std::move(Token));
		}
	}
	return TokenStream;
}

void dbc::LexicalAnalyzer::ResetTokenStream()
{
	while (TokenStream.size() > 0)
	{
		DeleteToken(TokenStream.front());
		TokenStream.pop_front();
	}
}

bool dbc::LexicalAnalyzer::CheckPair(char value)
{
	return CheckPair(value, value);;
}

bool dbc::LexicalAnalyzer::CheckPair(char value0, char value1)
{
	return (LastChar == value0 && PeekChar() == value1);
}

char dbc::LexicalAnalyzer::PeekChar(size_t Step)
{
	if (!Eof(Step)) return Source[Step + Index];
	return -1;
}

char dbc::LexicalAnalyzer::GetNextChar()
{
	if (!Eof()) return LastChar = Source[Index++];
	return -1;
}

void dbc::LexicalAnalyzer::SubOneIndex()
{
	--Index;
}

bool dbc::LexicalAnalyzer::Eof(size_t Step)
{
	return (Index + Step >= Length);
}

bool dbc::LexicalAnalyzer::ScanKeywordName(const char * Keyword)
{
	if (LastChar == Keyword[0])
	{
		int64 Index;
		int64 Length = strlen(Keyword);
		for (Index = 0; Index < Length; ++Index)
		{
			if (PeekChar(Index - 1) != Keyword[Index])
			{
				return false;
			}
		}
		for (Index = 0; Index < Length; ++Index)
		{
			GetNextChar();
		}
		SubOneIndex();
		return true;
	}
	return false;
}

bool dbc::LexicalAnalyzer::ScanCharInString(char Value, const char * Test)
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

bool dbc::LexicalAnalyzer::ScanEmptySpace()
{
	return (ScanWhitespace() || ScanComment());
}

bool dbc::LexicalAnalyzer::ScanSymbols(TokenPtr & Token)
{
	if (ScanNotEqu(Token)) return true;
	if (ScanLTEQU(Token)) return true;
	if (ScanGTEQU(Token)) return true;
	if (ScanEqu(Token)) return true;
	if (ScanLT(Token)) return true;
	if (ScanGT(Token)) return true;
	if (ScanPlus(Token)) return true;
	if (ScanMinus(Token)) return true;
	if (ScanSlash(Token)) return true;
	if (ScanAsterisk(Token)) return true;
	if (ScanComma(Token)) return true;
	if (ScanLParen(Token)) return true;
	if (ScanRParen(Token)) return true;
	return false;
}

bool dbc::LexicalAnalyzer::ScanKeywords(TokenPtr & Token)
{
	if (ScanWordFunc(Token)) return true;
	if (ScanWordIf(Token)) return true;
	if (ScanWordElseIf(Token)) return true;
	if (ScanWordElse(Token)) return true;
	if (ScanWordLet(Token)) return true;
	if (ScanWordEnd(Token)) return true;
	if (ScanWordRet(Token)) return true;
	if (ScanWordAnd(Token)) return true;
	if (ScanWordOr(Token)) return true;
	if (ScanWordBand(Token)) return true;
	if (ScanWordXor(Token)) return true;
	if (ScanWordBor(Token)) return true;
	if (ScanWordShl(Token)) return true;
	if (ScanWordShr(Token)) return true;
	if (ScanWordMod(Token)) return true;
	if (ScanWordWhile(Token)) return true;
	if (ScanWordNot(Token)) return true;
	return false;
}

bool dbc::LexicalAnalyzer::ScanTerminalSymbols(TokenPtr & Token)
{
	if (ScanBoolean(Token)) return true;
	if (ScanNumber(Token)) return true;
	if (ScanIdentifier(Token)) return true;
	return false;
}

#pragma region Whitespace
bool dbc::LexicalAnalyzer::ScanLinefeed(TokenPtr& Token)
{
	return false;
}
bool dbc::LexicalAnalyzer::ScanWhitespace()
{
	if (LastChar == LF)
	{
		++LineCount;
		return true;
	}
	return LastChar == 32 || LastChar == ' ' || LastChar == '\r' || LastChar == '\t';
}

bool dbc::LexicalAnalyzer::ScanComment()
{
	if (LastChar == '#')
	{
		while (LastChar != LF)
		{
			GetNextChar();
		}
		++LineCount;
		return true;
	}
	return false;
}
#pragma endregion
#pragma region Symbols
bool dbc::LexicalAnalyzer::ScanNotEqu(TokenPtr & Token)
{
	if (CheckPair('<', '>'))
	{		
		Token = NewToken(TokenType::SYM_NOTEQU);
		GetNextChar();
		return true;
	}
	return false;
}

bool dbc::LexicalAnalyzer::ScanEqu(TokenPtr & Token)
{
	if (LastChar == '=')
	{
		Token = NewToken(TokenType::SYM_EQU);
		return true;
	}
	return false;
}

bool dbc::LexicalAnalyzer::ScanLT(TokenPtr & Token)
{
	if (LastChar == '<')
	{
		Token = NewToken(TokenType::SYM_LT);
		return true;
	}
	return false;
}

bool dbc::LexicalAnalyzer::ScanGT(TokenPtr & Token)
{
	if (LastChar == '>')
	{
		Token = NewToken(TokenType::SYM_GT);
		return true;
	}
	return false;
}

bool dbc::LexicalAnalyzer::ScanLTEQU(TokenPtr & Token)
{
	if (CheckPair('<', '='))
	{
		Token = NewToken(TokenType::SYM_LTEQU);
		GetNextChar();
		return true;
	}
	return false;
}

bool dbc::LexicalAnalyzer::ScanGTEQU(TokenPtr & Token)
{
	if (CheckPair('>', '='))
	{
		Token = NewToken(TokenType::SYM_GTEQU);
		GetNextChar();
		return true;
	}
	return false;
}

bool dbc::LexicalAnalyzer::ScanPlus(TokenPtr & Token)
{
	if (LastChar == '+')
	{
		Token = NewToken(TokenType::SYM_PLUS);
		return true;
	}
	return false;
}

bool dbc::LexicalAnalyzer::ScanMinus(TokenPtr & Token)
{
	if (LastChar == '-')
	{
		Token = NewToken(TokenType::SYM_MINUS);
		return true;
	}
	return false;
}

bool dbc::LexicalAnalyzer::ScanSlash(TokenPtr & Token)
{
	if (LastChar == '/')
	{
		Token = NewToken(TokenType::SYM_SLASH);
		return true;
	}
	return false;
}

bool dbc::LexicalAnalyzer::ScanAsterisk(TokenPtr & Token)
{
	if (LastChar == '*')
	{
		Token = NewToken(TokenType::SYM_ASTERISK);
		return true;
	}
	return false;
}

bool dbc::LexicalAnalyzer::ScanComma(TokenPtr & Token)
{
	if (LastChar == ',')
	{
		Token = NewToken(TokenType::SYM_COMMA);
		return true;
	}
	return false;
}

bool dbc::LexicalAnalyzer::ScanLParen(TokenPtr & Token)
{
	if (LastChar == '(')
	{
		Token = NewToken(TokenType::SYM_LPAREN);
		return true;
	}
	return false;
}

bool dbc::LexicalAnalyzer::ScanRParen(TokenPtr & Token)
{
	if (LastChar == ')')
	{
		Token = NewToken(TokenType::SYM_RPAREN);
		return true;
	}
	return false;
}
#pragma endregion
#pragma region Keywords
bool dbc::LexicalAnalyzer::ScanWordNot(TokenPtr & Token)
{
	if (ScanKeywordName(KEYWORD_NOT))
	{
		Token = NewToken(TokenType::WORD_NOT);
		return true;
	}
	return false;
}

bool dbc::LexicalAnalyzer::ScanWordFunc(TokenPtr & Token)
{
	if (ScanKeywordName(KEYWORD_FUNC))
	{
		Token = NewToken(TokenType::WORD_FUNC);
		return true;
	}
	return false;
}

bool dbc::LexicalAnalyzer::ScanWordIf(TokenPtr & Token)
{
	if (ScanKeywordName(KEYWORD_IF))
	{
		Token = NewToken(TokenType::WORD_IF);
		return true;
	}
	return false;
}

bool dbc::LexicalAnalyzer::ScanWordElseIf(TokenPtr & Token)
{
	if (ScanKeywordName(KEYWORD_ELSEIF))
	{
		Token = NewToken(TokenType::WORD_ELSEIF);
		return true;
	}
	return false;
}

bool dbc::LexicalAnalyzer::ScanWordElse(TokenPtr & Token)
{
	if (ScanKeywordName(KEYWORD_ELSE))
	{
		Token = NewToken(TokenType::WORD_ELSE);
		return true;
	}
	return false;
}

bool dbc::LexicalAnalyzer::ScanWordLet(TokenPtr & Token)
{
	if (ScanKeywordName(KEYWORD_LET))
	{
		Token = NewToken(TokenType::WORD_LET);
		return true;
	}
	return false;
}

bool dbc::LexicalAnalyzer::ScanWordEnd(TokenPtr & Token)
{
	if (ScanKeywordName(KEYWORD_END))
	{
		Token = NewToken(TokenType::WORD_END);
		return true;
	}
	return false;
}

bool dbc::LexicalAnalyzer::ScanWordRet(TokenPtr & Token)
{
	if (ScanKeywordName(KEYWORD_RET))
	{
		Token = NewToken(TokenType::WORD_RET);
		return true;
	}
	return false;
}
bool dbc::LexicalAnalyzer::ScanWordAnd(TokenPtr & Token)
{
	if (ScanKeywordName(KEYWORD_AND))
	{
		Token = NewToken(TokenType::WORD_AND);
		return true;
	}
	return false;
}
bool dbc::LexicalAnalyzer::ScanWordOr(TokenPtr & Token)
{
	if (ScanKeywordName(KEYWORD_OR))
	{
		Token = NewToken(TokenType::WORD_OR);
		return true;
	}
	return false;
}
bool dbc::LexicalAnalyzer::ScanWordWhile(TokenPtr & Token)
{
	if (ScanKeywordName(KEYWORD_WHILE))
	{
		Token = NewToken(TokenType::WORD_WHILE);
		return true;
	}
	return false;
}
bool dbc::LexicalAnalyzer::ScanWordBand(TokenPtr & Token)
{
	if (ScanKeywordName(KEYWORD_BAND))
	{
		Token = NewToken(TokenType::WORD_BAND);
		return true;
	}
	return false;
}
bool dbc::LexicalAnalyzer::ScanWordMod(TokenPtr & Token)
{
	if (ScanKeywordName(KEYWORD_MOD))
	{
		Token = NewToken(TokenType::WORD_MOD);
		return true;
	}
	return false;
}
bool dbc::LexicalAnalyzer::ScanWordXor(TokenPtr & Token)
{
	if (ScanKeywordName(KEYWORD_XOR))
	{
		Token = NewToken(TokenType::WORD_XOR);
		return true;
	}
	return false;
}
bool dbc::LexicalAnalyzer::ScanWordBor(TokenPtr & Token)
{
	if (ScanKeywordName(KEYWORD_BOR))
	{
		Token = NewToken(TokenType::WORD_BOR);
		return true;
	}
	return false;
}
bool dbc::LexicalAnalyzer::ScanWordShl(TokenPtr & Token)
{
	if (ScanKeywordName(KEYWORD_SHL))
	{
		Token = NewToken(TokenType::WORD_SHL);
		return true;
	}
	return false;
}
bool dbc::LexicalAnalyzer::ScanWordShr(TokenPtr & Token)
{
	if (ScanKeywordName(KEYWORD_SHR))
	{
		Token = NewToken(TokenType::WORD_SHR);
		return true;
	}
	return false;
}
#pragma endregion
#pragma region Terminals
bool dbc::LexicalAnalyzer::ScanNumber(TokenPtr & Token)
{
	if (ScanCharInString(LastChar, WORD_DIGIT))
	{
		Token = NewToken(TokenType::NUMBER);
		Token->RawValue += LastChar;
		GetNextChar();
		while (ScanCharInString(LastChar, WORD_DIGIT))
		{
			Token->RawValue += LastChar;
			GetNextChar();
		}
		SubOneIndex();
		return true;
	}
	return false;
}
bool dbc::LexicalAnalyzer::ScanBoolean(TokenPtr & Token)
{
	if (ScanKeywordName(KEYWORD_TRUE))
	{
		Token = NewToken(TokenType::BOOLEAN);
		Token->RawValue = "1";
		return true;
	}
	else if (ScanKeywordName(KEYWORD_FALSE))
	{
		Token = NewToken(TokenType::BOOLEAN);
		Token->RawValue = "0";
		return true;
	}
	return false;
}
bool dbc::LexicalAnalyzer::ScanIdentifier(TokenPtr & Token)
{
	if (ScanCharInString(LastChar, WORD_ALPHA))
	{
		Token = NewToken(TokenType::IDENTIFIER);
		Token->RawValue += LastChar;
		GetNextChar();
		while (ScanCharInString(LastChar, WORD_ALPHA) ||
			   ScanCharInString(LastChar, WORD_DIGIT))
		{
			Token->RawValue += LastChar;
			GetNextChar();
		}
		SubOneIndex();
		return true;
	}
	return false;
}
#pragma endregion