#include <fstream>
#include <iostream>
#include <NIDAQmx.h>
#include <sstream>
#include <stdint.h>
#include <time.h>

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
	const unsigned int numberOfChannels = 1;
	const double bitResolutionOf9223 = 16.0;  // Double to ensure floating point arithmetic
	const double rangeOf9223Voltages = 21.2;  // Actual range of typical values read on NI 9223
	const char *physicalChannelNames = "cDAQ1Mod1/ai0";  // "cDAQ1Mod1/ai0:3, cDAQ1Mod2/ai0:3" would use eight channels
	const char *taskName = "myTask";
	const char *channelName = "testChannel";
	const int expectedLowValue = -10;
	const int expectedHighValue = 10;
	const double sampleRate = 10000;  // Sample rate in Hz per channel (NI-9223 has max sampling rate of 1 MHz)
	const unsigned int numberOfSecondsToRead = 15;
	const unsigned int sampsPerChanToRead = sampleRate * numberOfSecondsToRead;
	// 1GB of memory is close to the max amount you can have before the system claims you've used too much memory
	const unsigned int sizeOfReadArray = numberOfChannels * sampsPerChanToRead;
	const int sizeOfErrorString = 2048;
	int16_t *readArray = new int16[sizeOfReadArray];  // This will store our data (in unscaled binary resolution)
	int sampsPerChanRead;
	char errorString[sizeOfErrorString];

	// Create a "task" for this data acquisition
	DAQmxErrorCheck(DAQmxCreateTask(taskName, &task), errorString, sizeOfErrorString);

	// Create and add virtual channels to the task to indicate where to read data from
	DAQmxErrorCheck(DAQmxCreateAIVoltageChan(task, physicalChannelNames, channelName, DAQmx_Val_Diff, expectedLowValue, expectedHighValue, DAQmx_Val_Volts, NULL), errorString, sizeOfErrorString);

	// Configure timing parameters for reading
	DAQmxErrorCheck(DAQmxCfgSampClkTiming(task, NULL, sampleRate, DAQmx_Val_Rising, DAQmx_Val_FiniteSamps, sampsPerChanToRead), errorString, sizeOfErrorString);



	// Kick off the task
	DAQmxErrorCheck(DAQmxStartTask(task), errorString, sizeOfErrorString);

	// Get the time for the filename and read the data
	time_t rawtime;
	struct tm *timeinfo;
	char fileNameBuffer[200];
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(fileNameBuffer, 100,
			"/home/admin/SensorReader/output/%m-%d-%Y_%H_%M_%S", timeinfo);
	DAQmxErrorCheck(DAQmxReadBinaryI16(task, -1, DAQmx_Val_WaitInfinitely, DAQmx_Val_GroupByScanNumber, readArray, sizeOfReadArray, &sampsPerChanRead, NULL), errorString, sizeOfErrorString);

	// Clear the task out if it exists
	if(task != 0)
	{
		DAQmxStopTask(task);
		DAQmxClearTask(task);
	}

	// Write data out to file
	for (unsigned int i = 0; i < numberOfChannels; ++i)
	{
		std::stringstream ss;
		ss << fileNameBuffer << "-channel" << i << ".wav";
		std::string fileName = ss.str();
		WaveFile fp(readArray + (i * sampsPerChanRead), sampsPerChanRead, sampleRate);
		std::cout << "Writing: " << fileName << std::endl;
		fp.writeToFile(fileName);
	}

	std::cout << "\n" << sampsPerChanRead << " samples per channel read" << std::endl;
	delete readArray;

	return 0;
}
