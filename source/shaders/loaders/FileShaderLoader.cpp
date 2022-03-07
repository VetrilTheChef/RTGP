#include "FileShaderLoader.h"
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

///////////////////////////////////////////////////////////////////////////////
// PUBLIC
///////////////////////////////////////////////////////////////////////////////

FileShaderLoader::FileShaderLoader(std::string & newPath) noexcept :
	IShaderLoader()
{
	path = newPath;
}

FileShaderLoader::FileShaderLoader(std::string && newShaderPath) noexcept:
	FileShaderLoader(newShaderPath)
{
}

FileShaderLoader::~FileShaderLoader() noexcept
{
}

bool FileShaderLoader::load(std::string & source) noexcept
{
	ifstream fileStream(path, ios::in);

	if (fileStream.is_open())
	{
		stringstream buffer;
		string content;

		buffer << fileStream.rdbuf();
		source = buffer.str();

		fileStream.close();

		return true;
	}

	return false;
}

void FileShaderLoader::unload() noexcept
{
	// Not needed for this implementation
}

///////////////////////////////////////////////////////////////////////////////
// PROTECTED
///////////////////////////////////////////////////////////////////////////////