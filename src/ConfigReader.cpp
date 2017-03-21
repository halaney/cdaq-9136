#include <string>
#include "./ConfigReader.h"


ConfigReader::ConfigReader(const char *fileName)
	: configFile(fileName)
{
	read();
}


void ConfigReader::reRead()
{

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


void ConfigReader::read()
{

}
