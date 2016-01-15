#pragma once

namespace dbc
{
static const char LF = '\n';
static const char* WORD_DIGIT = "0123456789";
static const char* WORD_ALPHA = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";
static const char* WORD_BINDIGIT = "01";
static const char* WORD_HEXDIGIT = "0123456789abcdefABCDEF";

static const char* KEYWORD_IF = "if";
static const char* KEYWORD_ELSEIF = "elseif";
static const char* KEYWORD_ELSE = "else";
static const char* KEYWORD_FUNC = "func";
static const char* KEYWORD_RET = "return";
static const char* KEYWORD_END = "end";
static const char* KEYWORD_LET = "let";
static const char* KEYWORD_NOT = "not";
static const char* KEYWORD_AND = "and";
static const char* KEYWORD_OR = "or";
static const char* KEYWORD_IMPORT = "import";
static const char* KEYWORD_TRUE = "true";
static const char* KEYWORD_FALSE = "false";
static const char* KEYWORD_WHILE = "while";
// modulus
static const char* KEYWORD_MOD = "mod";
// shift left
static const char* KEYWORD_SHL = "shl";
// shift right
static const char* KEYWORD_SHR = "shr";
// bitwise or
static const char* KEYWORD_BOR = "ior";
// bitwise exclusive or
static const char* KEYWORD_XOR = "xor";
// bitwise and
static const char* KEYWORD_BAND = "band";
static const char SYMBOL_LTHAN = '<';
static const char SYMBOL_GTHAN = '>';
static const char SYMBOL_COMMA = ',';
static const char SYMBOL_LPAR = '(';
static const char SYMBOL_RPAR = ')';
static const char SYMBOL_DIVSYM = '/';
static const char SYMBOL_MULSYM = '*';
static const char SYMBOL_SUBSYM = '-';
static const char SYMBOL_ADDSYM = '+';
static const char SYMBOL_EQUAL = '=';
static const char SYMBOL_WS = ' ';
static const char SYMBOL_CR = '\r';
static const char SYMBOL_TAB = '\t';
} // namespace dbc