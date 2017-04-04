#include <iostream>
#include <string>
#include <time.h>

#include "../../reader/src/ConfigReader.h"
#include "./ConfigWriter.h"


int main(void)
{
	const std::string configFile = "/home/admin/reader/config/config.ini";
	ConfigReader reader(configFile);
	unsigned int startTime;
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

	// Get new config parameters
	std::cout << "Enter the start time (unix time): ";
	std::cin >> startTime;
	std::cout << "Enter the expected low value to be read (V): ";
	std::cin >> expectedLowValue;
	std::cout << "Enter the expected high value to be read (V): ";
	std::cin >> expectedHighValue;
	std::cout << "Enter the sampling rate... be wary of memory usage (Hz): ";
	std::cin >> sampleRate;
	std::cout << "Enter the time to read... be wary of memory usage (Hz): ";
	std::cin >> timeToRead;

	// Write the parameters
	ConfigWriter writer(configFile, startTime, expectedLowValue, expectedHighValue,
			sampleRate, timeToRead);
	writer.write();

	return 0;
}


