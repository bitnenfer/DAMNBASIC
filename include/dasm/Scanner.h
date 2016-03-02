#pragma once

#include <deque>
#include <dasm/Token.h>
#include <dasm/Words.h>

namespace dasm
{
class Scanner
{
public:
	void Scan(const std::string* Source);
	std::deque<TokenPtr>& GetTokenStream();
	bool ScanWhiteSpace();
	bool ScanComment();
	bool ScanKeyword(TokenPtr& Token);
	bool ScanSymbol(TokenPtr& Token);
	bool ScanTerminal(TokenPtr& Token);
	// Helpers
	bool ScanKeywordName(const char* Keyword);
	bool ScanCharInString(char Value, const char* Test);
	// Terminals
	bool ScanIdentifier(TokenPtr& Token);
	bool ScanNumber(TokenPtr& Token);
	bool ScanString(TokenPtr& Token);
	// Symbols
	bool ScanPlus(TokenPtr& Token);
	bool ScanMinus(TokenPtr& Token);
	bool ScanAsterisk(TokenPtr& Token);
	bool ScanSlash(TokenPtr& Token);
	bool ScanEquals(TokenPtr& Token);
	bool ScanLParen(TokenPtr& Token);
	bool ScanRParen(TokenPtr& Token);
	bool ScanPercent(TokenPtr& Token);
	bool ScanLT(TokenPtr& Token);
	bool ScanGT(TokenPtr& Token);
	bool ScanSemi(TokenPtr& Token);
	bool ScanColon(TokenPtr& Token);
	bool ScanHash(TokenPtr& Token);
	bool ScanDollar(TokenPtr& Token);
	bool ScanComma(TokenPtr& Token);
	bool ScanA(TokenPtr& Token);
	bool ScanX(TokenPtr& Token);
	bool ScanY(TokenPtr& Token);
	// Keywords
	bool ScanByte(TokenPtr& Token);
	bool ScanWord(TokenPtr& Token);
	bool ScanInclude(TokenPtr& Token);
	bool ScanMnemonic(TokenPtr& Token);

private:
	inline void DecIndex() { --Index; }
	bool Eof(int32 Step = 0);
	char Peek(int32 Step = 0);
	char GetNext();

	std::deque<TokenPtr> TokenStream;
	const std::string* Data;
	char Current;
	int32 Index;
	int32 Length;
	uint32 CurrentLine;
};
}