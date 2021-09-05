#include "ConfigParser.h"

ConfigParser::ConfigParser()
{
	mConfigFile = std::fstream("user.cfg", std::ios::in);

	if (!mConfigFile.is_open())
	{
		mConfigFile = std::fstream("user.cfg", std::ios::out);

		mConfigFile << "MUSIC_VOLUME=50\n";
		mConfigFile << "SOUND_VOLUME=50\n";

		mConfigFile.close();
	}

	mConfigFile = std::fstream("user.cfg", std::ios::in | std::ios::ate);
	
	if (!mConfigFile.is_open())
	{
		std::runtime_error("Failed to open user.cfg file!");
		return;
	}

	size_t fileSize = static_cast<size_t>(mConfigFile.tellg());
	mConfigFile.seekg(0);

	std::vector<char> buffer(fileSize);

	mConfigFile.read(buffer.data(), fileSize);

	mConfigsList = buffer.data();

	mConfigFile.close();
}

ConfigParser::~ConfigParser()
{
}
