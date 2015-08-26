#include "ddsCompiler.h"


ddsCompiler::ddsCompiler(void)
{
}


ddsCompiler::~ddsCompiler(void)
{
}

bool ddsCompiler::encode(const char* filename, const char* cfg)
{
	return false;
}

bool ddsCompiler::writeFile(const char* filename, const char* cfg)
{
	return false;
}

bool ddsCompiler::supportInput(const char* ext)
{
	//throw std::logic_error("The method or operation is not implemented.");
	if (!stricmp(ext, ".tga"))
	{
		return true;
	}
	return false;
}

bool ddsCompiler::supportOutput(const char* ext)
{
	//throw std::logic_error("The method or operation is not implemented.");
	if (!stricmp(ext, ".dds"))
	{
		return true;
	}
	return false;
}

const char* ddsCompiler::getName()
{
	return "ddsCompiler";
}