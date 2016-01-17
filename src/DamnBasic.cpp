#include <dbc/Builder.h>
#include <dbc/ArgsControl.h>

#define ArgError(Message, ...) \
	fprintf(stderr, Message, ##__VA_ARGS__); \
	exit(-1);

int main(int argc, const char* argv[])
{
	using namespace dbc;
#if _DEBUG || DEBUG
	gen::ASM6502CG Generator(true);
	Builder::Build("test/main.dbs", &Generator, true, true);
	Builder::SaveSourceFile("out.s65");
	Builder::Destoy();
	PAUSE;
#else
	if (argc > 1)
	{
		bool Verbose = false;
		bool HelpPrinted = false;
		const char* MainFile = nullptr;
		const char* OutFile = nullptr;
		const char* Gen = nullptr;
		CodeGenerator* Generator = nullptr;
		int Index;
		for (Index = 1; Index < argc; ++Index)
		{
			ArgsControl::Command Cmd = ArgsControl::GeCommand(argv[Index]);
			switch (Cmd)
			{
				case ArgsControl::Command::HELP:
					printf("=======================================\n");
					printf("DamnBASIC Compiler v0.0.1\n");
					printf("=======================================\n");
					printf("Author: Felipe Alfonso.\n");
					printf("Url: https://github.com/vptr/DAMNBASIC/\n");
					printf("=======================================\n");
					printf("Usage:\n");
					printf("\tdbc -[command] [argument]\n");
					printf("\nExample:\n");
					printf("\tdbc -i main.dbs -t 6502 -o output.asm\n");
					printf("\nCommands:\n");
					printf("\t-i Input file.\n");
					printf("\t-t Target [6502].\n");
					printf("\t-o Output file name (optional).\n");
					printf("\t-v Verbose print (optional).\n");
					HelpPrinted = true;
					break;
					break;
				case ArgsControl::Command::ADDFILE:
					if (Index + 1 < argc)
					{
						MainFile = argv[++Index];
						continue;
					}
					break;
				case ArgsControl::Command::VERBOSE:
					Verbose = true;
					continue;
					break;
				case ArgsControl::Command::TARGET:
					if (Index + 1 < argc)
					{
						Gen = argv[++Index];
						continue;
					}
				case ArgsControl::Command::OUTPUT:
					if (Index + 1 < argc && OutFile == nullptr)
					{
						OutFile = argv[++Index];
						continue;
					}
					continue;
					break;
				case ArgsControl::Command::NONE:
					ArgError("Invalid Argument %s", argv[Index]);
					break;
				default:
					ArgError("Invalid Argument %s", argv[Index]);
					break;
			}
		}
		if (!HelpPrinted)
		{
			GenType GType = ArgsControl::GetGenType(Gen);
			Generator = ArgsControl::GetGenerator(GType, Verbose);
			if (Generator == nullptr)
			{
				ArgError("Invalid target");
			}
			if (MainFile == nullptr)
			{
				ArgError("Missing input file.");
			}
			Builder::Build(MainFile, Generator, true, Verbose);
			if (OutFile == nullptr)
			{
				Builder::SaveSourceFile((std::string("out.") + ArgsControl::GetExt(GType)).c_str());
			}
			else
			{
				Builder::SaveSourceFile(OutFile);
			}
			Builder::Destoy();
		}
	}
	else
	{
		ArgError("Missing Input file.");
	}
#endif
	return 0;
}