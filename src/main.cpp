//============================================================================
// Name        : main.cpp
// Author      : Andrew Halaney
// Version     :
// Copyright   : Your copyright notice
// Description : Reads from the cDAQ-9136
//============================================================================

// Using stdio.h because of a weird windows bug with to_string() in iostream for MiniGW
#include <stdio.h>
#include <NIDAQmx.h>


// Checks to see if the API call returned an error and if it did what error
void DAQmxErrorCheck(int errorCode, char *errorString, int sizeOfErrorString)
{
	if(DAQmxFailed(errorCode))
	{
		DAQmxGetExtendedErrorInfo(errorString, sizeOfErrorString);
		printf("DAQmx Error: %s\n", errorString);
	}
}


int main(void)
{
	// Create necessary variables
	TaskHandle task;
	const unsigned int numberOfChannels = 1;
	const char *physicalChannelNames = "cDAQ1Mod1/ai0";  // "cDAQ1Mod1/ai0:3, cDAQ1Mod2/ai0:3" would use eight channels
	const char *taskName = "myTask";
	const char *channelName = "testChannel";
	const int expectedLowValue = 0;
	const int expectedHighValue = 1;
	const double sampleRate = 1000000;  // Sample rate in Hz per channel (NI-9223 has max sampling rate of 1 MHz)
	const unsigned int numberOfSecondsToRead = 125;
	const unsigned int sampsPerChanToRead = sampleRate * numberOfSecondsToRead;
	// 125x10^6 is close to the max amount of doubles you can have before the system claims you've used too much memory
	const unsigned int sizeOfReadArray = numberOfChannels * sampsPerChanToRead;
	const int sizeOfErrorString = 2048;
	double *readArray = new double[sizeOfReadArray];  // This will store our data
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

	// Read the data
	DAQmxErrorCheck(DAQmxReadAnalogF64(task, -1, DAQmx_Val_WaitInfinitely, DAQmx_Val_GroupByScanNumber, readArray, sizeOfReadArray, &sampsPerChanRead, NULL), errorString, sizeOfErrorString);

	// Clear the task out if it exists
	if(task != 0)
	{
		DAQmxStopTask(task);
		DAQmxClearTask(task);
	}

	printf("\n%d samples per channel read supposedly\n", sampsPerChanRead);
	delete readArray;
	return 0;
}
