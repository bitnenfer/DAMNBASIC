#pragma once
#include <string>
#include <fstream>
#include <vector>

namespace dbc
{
std::string LoadFile(const char *Path);
void SaveFile(const char* Path, const std::string& FileSource);
void SaveBinaryFile(const char * Path, std::vector<unsigned char>& BinData);
}