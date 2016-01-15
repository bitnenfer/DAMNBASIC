#include <dbc/FileLoader.h>
#include <dbc/SyntaxAnalyzer.h>
#include <dbc/SemanticAnalyzer.h>
#include <dbc/Debug.h>
#include <dbc/gen/ASM6502CG.h>

#pragma optimize("", off)
int main(int argc, const char* argv[])
{
	using namespace dbc;
#if _DEBUG || DEBUG
	const char* File = "test/main.dbs";
#else
	const char* File = "main.dbs";
#endif
	std::string Source = LoadFile(File);
	SyntaxAnalyzer Parser(Source, File);
	std::vector<Scope> ScopeStack;
	SemanticAnalyzer Semant(File, &ScopeStack, true);
	LeafPtr SyntaxTree = Parser.Analyze();
	Semant.Analyze(SyntaxTree);
	CodeGenerator* Generator = new gen::ASM6502CG();
	Generator->Generate(SyntaxTree, File, true);
	std::string TransCode = Generator->GetTransSource();
	SaveFile("out.s65", TransCode);
	//printf("%s\n\n", TransCode.c_str());
//	PrintSyntaxTree(SyntaxTree);
	PAUSE;
	return 0;
}