#pragma once
#include <dbc/Token.h>
#include <dbc/LeafNode.h>

namespace dbc
{
void PrintSyntaxTree(LeafPtr SyntaxTree, const char* Side = "ROOT", uint64 Deep = 0);
void PrintToken(TokenPtr Token);
void PrintLeaf(LeafPtr Node);
}