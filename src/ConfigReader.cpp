#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <string>
#include "./ConfigReader.h"

/* Example configuration file layout
 * startTime=100
 * lowValue=-10
 * highValue=10
 * sampleRate=500000
 * timeToRead=30
 */


ConfigReader::ConfigReader(const std::string fileName)
	: configFile(fileName)
{
	read();
}


void ConfigReader::read()
{
	std::string line;
	std::string subString;
	std::ifstream fp(configFile.c_str());

	std::getline(fp, line);
	while (!fp.eof())
	{
		if (line.find("startTime=") != std::string::npos)
		{
			subString = line.substr(line.find("startTime=") + strlen("startTime="));
			startTime = atoi(subString.c_str());
		}
		else if (line.find("lowValue=") != std::string::npos)
		{
			subString = line.substr(line.find("lowValue=") + strlen("lowValue="));
			expectedLowValue = atoi(subString.c_str());
		}
		else if (line.find("highValue=") != std::string::npos)
		{
			subString = line.substr(line.find("highValue=") + strlen("highValue="));
			expectedHighValue = atoi(subString.c_str());
		}
		else if (line.find("sampleRate=") != std::string::npos)
		{
			subString = line.substr(line.find("sampleRate=") + strlen("sampleRate="));
			sampleRate = atoi(subString.c_str());
		}
		else if (line.find("timeToRead=") != std::string::npos)
		{
			subString = line.substr(line.find("timeToRead=") + strlen("timeToRead="));
			timeToRead = atoi(subString.c_str());
		}
		else
		{
			// This line didn't have any of our config words
		}

		line.erase();
		subString.erase();
		std::getline(fp, line);
	}

	fp.close();
}


void ConfigReader::reRead()
{
	read();
}


unsigned int ConfigReader::getStartTime() const
{
	return startTime;
}


int ConfigReader::getExpectedLowValue() const
{
	return expectedLowValue;
}

int ConfigReader::getExpectedHighValue() const
{
	return expectedHighValue;
}


unsigned int ConfigReader::getSampleRate() const
{
	return sampleRate;
}

unsigned int ConfigReader::getTimeToRead() const
{
	return timeToRead;
}

