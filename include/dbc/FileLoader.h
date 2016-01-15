#pragma once
#include <string>
#include <fstream>

namespace dbc
{
std::string LoadFile(const char *Path);
void SaveFile(const char* Path, const std::string& FileSource);
}