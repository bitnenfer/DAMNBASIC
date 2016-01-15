#pragma once

#include <vector>
#include <dbc/LeafNode.h>

namespace dbc
{
struct Scope
{
	std::vector<LeafPtr> SymbolTable;
};
}