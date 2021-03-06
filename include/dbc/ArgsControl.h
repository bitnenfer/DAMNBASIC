#pragma once
#include <cstring>
// Generators
#include <dbc/gen/ASM6502CG.h>

namespace dbc
{
namespace ArgsControl
{
enum class Command
{
	ADDFILE,
	VERBOSE,
	TARGET,
	OUTPUT,
	HELP,
	ASMOUT,
	NONE
};

constexpr char COMMAND_ADDFILE[] = "-i";
constexpr char COMMAND_VERBOSE[] = "-v";
constexpr char COMMAND_TARGET[] = "-t";
constexpr char COMMAND_OUTPUT[] = "-o";
constexpr char COMMAND_HELP[] = "-h";
constexpr char COMMAND_ASMOUT[] = "-s";

constexpr char TYPE_ASM65[] = "6502";

Command GeCommand(const char* Arg)
{
	if (Arg == nullptr)
	{
		return Command::NONE;
	}
	if (strcmp(Arg, COMMAND_HELP) == 0)
	{
		return Command::HELP;
	}
	else if (strcmp(Arg, COMMAND_ADDFILE) == 0)
	{
		return Command::ADDFILE;
	}
	else if (strcmp(Arg, COMMAND_VERBOSE) == 0)
	{
		return Command::VERBOSE;
	}
	else if (strcmp(Arg, COMMAND_TARGET) == 0)
	{
		return Command::TARGET;
	}
	else if (strcmp(Arg, COMMAND_OUTPUT) == 0)
	{
		return Command::OUTPUT;
	}
	else if (strcmp(Arg, COMMAND_ASMOUT) == 0)
	{
		return Command::ASMOUT;
	}
	return Command::NONE;
}
GenType GetGenType(const char* Arg)
{
	if (Arg == nullptr)
	{
		return GenType::NONE;
	}
	else if (strcmp(Arg, TYPE_ASM65) == 0)
	{
		return GenType::ASM6502_BUILD;
	}
	return GenType::NONE;
}
const char* GetExt(GenType Type)
{
	switch (Type)
	{
		case GenType::ASM6502_BUILD:
			return "prg";
			break;
		case GenType::ASM6502_SRC:
			return "asm";
			break;
		case GenType::NONE:
			break;
		default:
			break;
	}
	return nullptr;
}
CodeGenerator* GetGenerator(GenType Type, bool Verbose = false)
{
	switch (Type)
	{
		case GenType::ASM6502_BUILD:
			return new gen::ASM6502CG(Verbose);
			break;
		case GenType::NONE:
			break;
		default:
			break;
	}
	return nullptr;
}
}
}