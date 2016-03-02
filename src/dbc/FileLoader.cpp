#include <dbc/FileLoader.h>

void dbc::SaveFile(const char* Path, const std::string& FileSource)
{
	std::ofstream File;
	File.open(Path, std::ios::trunc | std::ios::out);
	File << FileSource;
	File.close();
}

std::string dbc::LoadFile(const char *Path)
{
	std::ifstream File;
	File.open(Path);
	if (File.fail())
	{
		fprintf(stderr, "Can't load file %s.\n", Path);
		getchar();
		exit(-1);
	}
	std::string str((std::istreambuf_iterator<char>(File)), std::istreambuf_iterator<char>());
	File.close();
	return str + "\n";
}

void dbc::SaveBinaryFile(const char * Path, std::vector<unsigned char>& BinData)
{
	std::ofstream File;
	File.open(Path, std::ios::trunc | std::ios::out | std::ios::binary);
	for (unsigned char byte : BinData)
	{
		File << byte;
	}
	File.close();
}