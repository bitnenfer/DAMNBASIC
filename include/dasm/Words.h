#pragma once

namespace dasm
{
constexpr int32 MNEMONICS_COUNT = 56;
static const char* KEYWORD_MNEMONICS[MNEMONICS_COUNT] =
{
	"ADC",
	"AND",
	"ASL",
	"BIT",
	"BPL",
	"BMI",
	"BVC",
	"BVS",
	"BCC",
	"BCS",
	"BNE",
	"BEQ",
	"BRK",
	"CMP",
	"CPX",
	"CPY",
	"DEC",
	"EOR",
	"CLC",
	"SEC",
	"CLI",
	"SEI",
	"CLV",
	"CLD",
	"SED",
	"INC",
	"JMP",
	"JSR",
	"LDA",
	"LDX",
	"LDY",
	"LSR",
	"NOP",
	"ORA",
	"TAX",
	"TXA",
	"DEX",
	"INX",
	"TAY",
	"TYA",
	"DEY",
	"INY",
	"ROR",
	"ROL",
	"RTI",
	"RTS",
	"SBC",
	"STA",
	"TXS",
	"TSX",
	"PHA",
	"PLA",
	"PHP",
	"PLP",
	"STX",
	"STY"
};
static constexpr char KEYWORD_INCLUDE[] = ".INCLUDE";
static constexpr char KEYWORD_BYTE[] = ".BYTE";
static constexpr char KEYWORD_WORD[] = ".WORD";
static constexpr char SYMBOL_LPAREN = '(';
static constexpr char SYMBOL_RPAREN = ')';
static constexpr char SYMBOL_EQUAL = '=';
static constexpr char SYMBOL_PLUS = '+';
static constexpr char SYMBOL_MINUS = '-';
static constexpr char SYMBOL_ASTERISK = '*';
static constexpr char SYMBOL_SLASH = '/';
static constexpr char SYMBOL_LF = '\n';
static constexpr char SYMBOL_PERCENT = '%';
static constexpr char SYMBOL_LT = '<';
static constexpr char SYMBOL_GT = '>';
static constexpr char SYMBOL_COLON = ':';
static constexpr char SYMBOL_SEMICOLON = ';';
static constexpr char SYMBOL_HASH = '#';
static constexpr char SYMBOL_DOLLAR = '$';
static constexpr char SYMBOL_COMMA = ',';
static constexpr char SYMBOL_CR = '\r';
static constexpr char SYMBOL_TB = '\t';
static constexpr char SYMBOL_SPACE = ' ';
static constexpr char WORD_DIGIT[] = "0123456789";
static constexpr char WORD_ALPHA[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";
static constexpr char WORD_BINDIGIT[] = "01";
static constexpr char WORD_HEXDIGIT[] = "0123456789abcdefABCDEF";
static constexpr char WORD_HEXALPHA[] = "abcdefABCDEF";

}