#pragma once
#include <dasm/Node.h>
#include <dasm/Token.h>
#include <deque>

namespace dasm
{
class Parser
{
public:
	NodePtr Parse(std::deque<TokenPtr>* TokenStream);
	NodePtr ParseProgram();
	// Line
	NodePtr ParseLine();
	NodePtr ParseInstruction();
	NodePtr ParseDirective();
	NodePtr ParseDeclVar();
	NodePtr ParseLabel();
	NodePtr ParseOrigin();
	// Leafs
	NodePtr ParseAddressMode();
	// Directives
	NodePtr ParseByteDirective();
	NodePtr ParseWordDirective();
	NodePtr ParseIncludeDirective();
	// Constants
	NodePtr ParseAddress();
	NodePtr ParseValue();
	NodePtr ParseHexAddress();
	NodePtr ParseHexValue();
	NodePtr ParseIdentAddress();
	NodePtr ParseIdentValue();
	NodePtr ParseDecValue();
	NodePtr ParseString();

private:
	void PullToken();
	TokenPtr PeekToken(int32 Offset = 0);

	TokenPtr CurrentToken = nullptr;
	std::deque<TokenPtr>* TokenStream = nullptr;
};
}