#include <dbc/Builder.h>
#include <cstdio>

using namespace dbc;

std::string Builder::TransData;
std::vector<SyntaxAnalyzer*> Builder::Parsers;
std::vector<SemanticAnalyzer*> Builder::SemanticCheckers;
std::vector<Scope> Builder::ScopeStack;
std::vector<const char*> Builder::LoadedPaths;

void dbc::Builder::Build(const char* File, CodeGenerator* Generator, bool IsMain, bool Verbose)
{
	if (!AllowLoading(File))
	{
		if (!IsMain && Verbose) printf("Importing %s\n", File);
		std::string DamnBasicSource = LoadFile(File);
		if (IsMain && Verbose) printf("Scanning...\n");
		SyntaxAnalyzer* Parser = new SyntaxAnalyzer(DamnBasicSource, File);
		SemanticAnalyzer* Semant = new SemanticAnalyzer(File, &ScopeStack, IsMain);
		if (IsMain && Verbose) printf("Parsing...\n");
		LeafPtr Tree = Parser->Analyze();
		if (IsMain && Verbose) printf("Semantic Checking...\n");
		Semant->Analyze(Tree);
		Generator->Generate(Tree, File, IsMain);
		TransData += Generator->GetTransSource();
		Parsers.push_back(Parser);
		SemanticCheckers.push_back(Semant);
		if (IsMain && Verbose) printf("Compilation Successful.\n");
	}
}

void dbc::Builder::SaveSourceFile(const char* OutputFileName)
{
	SaveFile(OutputFileName, Builder::TransData);
}

void dbc::Builder::Destoy()
{
	for (SyntaxAnalyzer* Parser : Parsers)
	{
		delete Parser;
	}
	for (SemanticAnalyzer* Semant : SemanticCheckers)
	{
		delete Semant;
	}
	ScopeStack.clear();
}

bool dbc::Builder::AllowLoading(const char * Path)
{
	for (const char* CheckPath : LoadedPaths)
	{
		if (strcmp(CheckPath, Path) == 0)
		{
			return true;
		}
	}
	return false;
}
