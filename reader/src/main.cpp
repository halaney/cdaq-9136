#include <fstream>
#include <iostream>
#include <NIDAQmx.h>
#include <sstream>
#include <stdint.h>
#include <time.h>
#include <unistd.h>

#include "../../shared/ConfigReader.h"
#include "./Wave.h"


// Checks to see if the API call returned an error and if it did what error
void DAQmxErrorCheck(int errorCode, char *errorString, int sizeOfErrorString)
{
	if(DAQmxFailed(errorCode))
	{
		DAQmxGetExtendedErrorInfo(errorString, sizeOfErrorString);
		std::cout << "DAQmx Error: " << errorString << std::endl;
	}
}


int main(void)
{
	// Create necessary variables
	TaskHandle task;
	const unsigned int numberOfChannels = 8;
	const char *physicalChannelNames = "cDAQ1Mod1/ai0:3, cDAQ1Mod2/ai0:3";  // "cDAQ1Mod1/ai0:3, cDAQ1Mod2/ai0:3" would use eight channels
	const char *taskName = "myTask";
	const int sizeOfErrorString = 2048;
	char errorString[sizeOfErrorString];
	int sampsPerChanRead;

	// Read configuration file
	ConfigReader reader("/home/admin/reader/config/config.ini");
	const double expectedLowValue = reader.getExpectedLowValue();
	const double expectedHighValue = reader.getExpectedHighValue();
	const unsigned int sampleRate = reader.getSampleRate();  // Sample rate in Hz per channel (NI-9223 has max sampling rate of 1 MHz)
	const unsigned int numberOfSecondsToRead = reader.getTimeToRead();
	const unsigned int startTime = reader.getStartTime();
	const unsigned int sampsPerChanToRead = sampleRate * numberOfSecondsToRead;
	const unsigned int sizeOfReadArray = numberOfChannels * sampsPerChanToRead;
	reader.display();

	// Wait till it is 5 seconds before startTime
	// This way we aren't holding resources and creating tasks without properly closing them if the daemon is restarted
	time_t rawTime;
	time(&rawTime);
	if ((startTime - rawTime - 5) > 0)
	{
		sleep(startTime - rawTime - 5);
	}
	else if (rawTime > startTime + 3600)  // If it's way passed start time don't bother
	{
		std::cout << "startTime was over an hour ago... aborting" << std::endl;
		return 0;
	}
	std::cout << "Number of bytes in data array: " << sizeOfReadArray * sizeof(int16_t) << std::endl;
	// 1GB of memory is close to the max amount you can have before the system claims you've used too much memory
	int16_t *readArray = new int16[sizeOfReadArray];  // This will store our data (in unscaled binary resolution)

	// Create a "task" for this data acquisition
	DAQmxErrorCheck(DAQmxCreateTask(taskName, &task), errorString, sizeOfErrorString);

	// Create and add virtual channels to the task to indicate where to read data from
	DAQmxErrorCheck(DAQmxCreateAIVoltageChan(task, physicalChannelNames, "", DAQmx_Val_Diff,
			expectedLowValue, expectedHighValue, DAQmx_Val_Volts, NULL), errorString, sizeOfErrorString);

	// Configure timing parameters for reading
	DAQmxErrorCheck(DAQmxCfgSampClkTiming(task, NULL, sampleRate, DAQmx_Val_Rising,
			DAQmx_Val_FiniteSamps, sampsPerChanToRead), errorString, sizeOfErrorString);

	// Kick off the task
	DAQmxErrorCheck(DAQmxStartTask(task), errorString, sizeOfErrorString);

	// Wait till actual time to read
	time(&rawTime);
	if (startTime - rawTime > 0)
	{
		sleep(startTime - rawTime);
	}

	// Get the time for the filename and read the data
	struct tm *timeinfo;
	char fileNameBuffer[200];
	time(&rawTime);
	timeinfo = localtime(&rawTime);
	strftime(fileNameBuffer, 100,
			"/home/admin/reader/output/%m-%d-%Y_%H_%M_%S", timeinfo);
	// Perform the read
	DAQmxErrorCheck(DAQmxReadBinaryI16(task, -1, DAQmx_Val_WaitInfinitely, DAQmx_Val_GroupByChannel,
			readArray, sizeOfReadArray, &sampsPerChanRead, NULL), errorString, sizeOfErrorString);

	// Clear the task out if it exists
	if (task != NULL)
	{
		DAQmxStopTask(task);
		DAQmxClearTask(task);
	}

	// Write data out to file
	clock_t beginWriteTime = clock();
	for (unsigned int i = 0; i < numberOfChannels; ++i)
	{
		std::stringstream ss;
		ss << fileNameBuffer << "-channel" << i << ".wav";
		std::string fileName = ss.str();
		WaveFile fp(readArray + (i * sampsPerChanRead), sampsPerChanRead, sampleRate);
		std::cout << "Writing: " << fileName << std::endl;
		fp.writeToFile(fileName);
	}
	clock_t endWriteTime = clock();
	double timeSpent = (double)(endWriteTime - beginWriteTime) / CLOCKS_PER_SEC;
	std::cout << "Time to write files in seconds: " << timeSpent << std::endl;

	std::cout << "\n" << sampsPerChanRead << " samples per channel read" << std::endl;
	delete readArray;

	return 0;
}
