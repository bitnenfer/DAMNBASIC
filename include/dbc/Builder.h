#pragma once
#include <dbc/SyntaxAnalyzer.h>
#include <dbc/SemanticAnalyzer.h>
#include <dbc/CodeGenerator.h>
#include <dbc/FileLoader.h>

namespace dbc
{
class Builder
{
public:
	static void Build(const char* File, CodeGenerator* Generator, bool IsMain = false, bool Verbose = false);
	static void SaveSourceFile(const char* OutputFileName);
	static void Destoy();

private:
	static bool AllowLoading(const char* Path);

	static std::vector<Scope> ScopeStack;
	static std::string TransData;
	static std::vector<SyntaxAnalyzer*> Parsers;
	static std::vector<SemanticAnalyzer*> SemanticCheckers;
	static std::vector<const char*> LoadedPaths;
};
}