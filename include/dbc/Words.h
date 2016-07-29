#pragma once

namespace dbc
{
static constexpr char LF = '\n';
static constexpr char WORD_DIGIT[] = "0123456789";
static constexpr char WORD_ALPHA[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";
static constexpr char WORD_BINDIGIT[] = "01";
static constexpr char WORD_HEXDIGIT[] = "0123456789abcdefABCDEF";

static constexpr char KEYWORD_IF[] = "if";
static constexpr char KEYWORD_ELSEIF[] = "elseif";
static constexpr char KEYWORD_ELSE[] = "else";
static constexpr char KEYWORD_FUNC[] = "func";
static constexpr char KEYWORD_RET[] = "return";
static constexpr char KEYWORD_END[] = "end";
static constexpr char KEYWORD_LET[] = "let";
static constexpr char KEYWORD_NOT[] = "not";
static constexpr char KEYWORD_AND[] = "and";
static constexpr char KEYWORD_OR[] = "or";
static constexpr char KEYWORD_IMPORT[] = "import";
static constexpr char KEYWORD_TRUE[] = "true";
static constexpr char KEYWORD_FALSE[] = "false";
static constexpr char KEYWORD_WHILE[] = "while";
static constexpr char KEYWORD_NATIVE[] = "native";
// modulus
static constexpr char KEYWORD_MOD[] = "mod";
// shift left
static constexpr char KEYWORD_SHL[] = "shl";
// shift right
static constexpr char KEYWORD_SHR[] = "shr";
// bitwise or
static constexpr char KEYWORD_BOR[] = "ior";
// bitwise exclusive or
static constexpr char KEYWORD_XOR[] = "xor";
// bitwise and
static constexpr char KEYWORD_BAND[] = "band";
static constexpr char SYMBOL_LTHAN = '<';
static constexpr char SYMBOL_GTHAN = '>';
static constexpr char SYMBOL_COMMA = ',';
static constexpr char SYMBOL_LPAR = '(';
static constexpr char SYMBOL_RPAR = ')';
static constexpr char SYMBOL_DIVSYM = '/';
static constexpr char SYMBOL_MULSYM = '*';
static constexpr char SYMBOL_SUBSYM = '-';
static constexpr char SYMBOL_ADDSYM = '+';
static constexpr char SYMBOL_EQUAL = '=';
static constexpr char SYMBOL_WS = ' ';
static constexpr char SYMBOL_CR = '\r';
static constexpr char SYMBOL_TAB = '\t';
} // namespace dbc