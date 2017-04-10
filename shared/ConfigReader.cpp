#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <time.h>

#include "./ConfigReader.h"

/* Example configuration file layout
 * startTime=100
 * lowValue=-10.0
 * highValue=10.0
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

	for (std::string line; std::getline(fp, line); )
	{
		if (line.find("startTime=") != std::string::npos)
		{
			subString = line.substr(line.find("startTime=") + strlen("startTime="));
			startTime = atoi(subString.c_str());
		}
		else if (line.find("lowValue=") != std::string::npos)
		{
			subString = line.substr(line.find("lowValue=") + strlen("lowValue="));
			expectedLowValue = atof(subString.c_str());
		}
		else if (line.find("highValue=") != std::string::npos)
		{
			subString = line.substr(line.find("highValue=") + strlen("highValue="));
			expectedHighValue = atof(subString.c_str());
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


void ConfigReader::display()
{
	time_t rawStartTime = startTime;
	struct tm *localTime = localtime(&rawStartTime);

	std::cout << "\nCurrent Configuration Settings" << std::endl;
	std::cout <<   "------------------------------" << std::endl;
	std::cout << "Start time (unix time): " << startTime << std::endl;
	std::cout << "Start time (cDAQ-9136 local time): " << asctime(localTime);
	std::cout << "Sample Rate (Hz): " << sampleRate << std::endl;
	std::cout << "Time to read (seconds): " << timeToRead << std::endl;
	std::cout << "Expected low value (V): " << expectedLowValue << std::endl;
	std::cout << "Expected high value (V): " << expectedHighValue << "\n" << std::endl;
}


unsigned int ConfigReader::getStartTime() const
{
	return startTime;
}


double ConfigReader::getExpectedLowValue() const
{
	return expectedLowValue;
}

double ConfigReader::getExpectedHighValue() const
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

