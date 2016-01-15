#pragma once

#include <dbc/Token.h>
#include <dbc/Words.h>
#include <deque>
#include <memory>

namespace dbc
{
class LexicalAnalyzer
{
public:
	LexicalAnalyzer(const char* File);
	~LexicalAnalyzer();
	std::deque<TokenPtr>& Analyze(const std::string& _Source);

protected:
	void ResetTokenStream();
	bool CheckPair(char value);
	bool CheckPair(char value0, char value1);
	char PeekChar(size_t Step = 0);
	char GetNextChar();
	void SubOneIndex();
	bool Eof(size_t Step = 0);
	bool ScanKeywordName(const char* Keyword);
	bool ScanCharInString(char Value, const char* Test);
	// Scanning Method Collection.
	bool ScanEmptySpace();
	bool ScanSymbols(TokenPtr& Token);
	bool ScanKeywords(TokenPtr& Token);
	bool ScanTerminalSymbols(TokenPtr& Token);
	// Whitespaces
	bool ScanLinefeed(TokenPtr& Token);
	bool ScanWhitespace();
	bool ScanComment();
	// Symbols
	bool ScanNotEqu(TokenPtr& Token);
	bool ScanEqu(TokenPtr& Token);
	bool ScanLT(TokenPtr& Token);
	bool ScanGT(TokenPtr& Token);
	bool ScanLTEQU(TokenPtr& Token);
	bool ScanGTEQU(TokenPtr& Token);
	bool ScanPlus(TokenPtr& Token);
	bool ScanMinus(TokenPtr& Token);
	bool ScanSlash(TokenPtr& Token);
	bool ScanAsterisk(TokenPtr& Token);
	bool ScanComma(TokenPtr& Token);
	bool ScanLParen(TokenPtr& Token);
	bool ScanRParen(TokenPtr& Token);
	// Keywords
	bool ScanWordNot(TokenPtr& Token);
	bool ScanWordFunc(TokenPtr& Token);
	bool ScanWordIf(TokenPtr& Token);
	bool ScanWordElseIf(TokenPtr& Token);
	bool ScanWordElse(TokenPtr& Token);
	bool ScanWordLet(TokenPtr& Token);
	bool ScanWordEnd(TokenPtr& Token);
	bool ScanWordRet(TokenPtr& Token);
	bool ScanWordAnd(TokenPtr& Token);
	bool ScanWordOr(TokenPtr& Token);
	bool ScanWordWhile(TokenPtr& Token);
	bool ScanWordBand(TokenPtr& Token);
	bool ScanWordMod(TokenPtr& Token);
	bool ScanWordXor(TokenPtr& Token);
	bool ScanWordBor(TokenPtr& Token);
	bool ScanWordShl(TokenPtr& Token);
	bool ScanWordShr(TokenPtr& Token);

	// Terminals
	bool ScanNumber(TokenPtr& Token);
	bool ScanBoolean(TokenPtr& Token);
	bool ScanIdentifier(TokenPtr& Token);

private:
	std::deque<TokenPtr> TokenStream;
	std::string Source;
	uint64 Index;
	uint64 Length;
	uint64 LineCount;
	char LastChar;
	const char* SourcePath;
};
} // namespace dbc
