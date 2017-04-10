#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <time.h>

#include "../../shared/ConfigReader.h"
#include "./ConfigWriter.h"


// Updates startTime to unix equivalent of timeString
// Returns true if success, false if not
// String must be of format YYYY:MM:DD:hh:mm:ss
bool convertStringToUnixTime(std::string timeString, time_t &startTime)
{
	// Set up startTime temporary variables
	std::stringstream timeStream(timeString);
	std::string token;
	std::vector<long int> startTimeVector;
	struct tm startTimeStruct;

	// Get individual time parameters
	while (std::getline(timeStream, token, ':'))  // Get each field and convert to int
	{
		startTimeVector.push_back(atol(token.c_str()));
	}
	if (startTimeVector.size() != 6)
	{
		std::cout << "Incorrect date entry." << std::endl;
		return false;
	}
	else
	{
		startTimeStruct.tm_year = startTimeVector[0] - 1900;  // Year since 1900
		startTimeStruct.tm_mon = startTimeVector[1] - 1;  // Month (zero based)
		startTimeStruct.tm_mday = startTimeVector[2];
		startTimeStruct.tm_hour = startTimeVector[3];
		startTimeStruct.tm_min = startTimeVector[4];
		startTimeStruct.tm_sec = startTimeVector[5];
		startTime = mktime(&startTimeStruct);
		return true;
	}
}


int main(void)
{
	const std::string configFile = "/home/admin/reader/config/config.ini";
	ConfigReader reader(configFile);
	std::string timeString;
	time_t startTime;
	double expectedLowValue;
	double expectedHighValue;
	unsigned int sampleRate;
	unsigned int timeToRead;

	// Display title
	std::cout << "\n\ncDAQ-9136 Input Configuration Editor" << std::endl;
	std::cout <<     "------------------------------------" << std::endl;
	std::cout << std::endl;

	// Display current config
	reader.display();

	std::cout << "Enter the start time (YYYY:MM:DD:hh:mm:ss): ";
	std::cin >> timeString;
	if (!convertStringToUnixTime(timeString, startTime))
	{
		std::cout << "Incorrect time entry." << std::endl;
		return 1;
	}
	std::cout << "Enter the expected low value to be read (V): ";
	std::cin >> expectedLowValue;
	std::cout << "Enter the expected high value to be read (V): ";
	std::cin >> expectedHighValue;
	std::cout << "Enter the sampling rate... be wary of memory usage (Hz): ";
	std::cin >> sampleRate;
	std::cout << "Enter the time to read... be wary of memory usage (s): ";
	std::cin >> timeToRead;

	// Write the parameters
	ConfigWriter writer(configFile, startTime, expectedLowValue, expectedHighValue,
			sampleRate, timeToRead);
	writer.write();

	// Redisplay new parameters
	reader.reRead();
	reader.display();

	return 0;
}


