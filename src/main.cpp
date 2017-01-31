//============================================================================
// Name        : main.cpp
// Author      : Andrew Halaney
// Version     :
// Copyright   : Your copyright notice
// Description : Reads from the cDAQ-9136
//============================================================================

// Using stdio.h because of a weird windows bug with to_string() in iostream
#include <stdio.h>
#include <NIDAQmx.h>

//TODO: Wrap all DAQmx calls in a macro that checks for return error
#define DAQmxErrChk(functionCall) if(DAQmxFailed(error=(functionCall))) goto Error; else

int main(void) {
	printf("Hello World!\n");

	TaskHandle task;
	int error = 0;
	const unsigned int numberOfChannels = 1;
	const char *physicalChannelNames = "cDAQ1Mod1/ai0";  // "cDAQ1Mod1/ai0:3, cDAQ1Mod2/ai0:3"
	const char *taskName = "myTask";
	const char *channelName = "testChannel";
	const int expectedLowValue = 0;
	const int expectedHighValue = 1;
	const double sampleRate = 1000000;  // Sample rate in Hz per channel
	const unsigned int numberOfSecondsToRead = 180;
	const unsigned int sampsPerChanToRead = sampleRate * numberOfSecondsToRead;
	const unsigned int sizeOfReadArray = numberOfChannels * sampsPerChanToRead;
	double *readArray = new double[sizeOfReadArray];
	int sampsPerChanRead;
	//const double triggerLevel = .8;
	char errorString[2048];

	DAQmxErrChk(DAQmxCreateTask(taskName, &task));
	DAQmxErrChk(DAQmxCreateAIVoltageChan(task, physicalChannelNames, channelName, DAQmx_Val_Diff, expectedLowValue, expectedHighValue, DAQmx_Val_Volts, NULL));
	DAQmxErrChk(DAQmxCfgSampClkTiming(task, NULL, sampleRate, DAQmx_Val_Rising, DAQmx_Val_FiniteSamps, sampsPerChanToRead));
	DAQmxErrChk(DAQmxStartTask(task));

	//double timeToWait = 30;  // wait 30 seconds max to trigger and read data
	//DAQmxErrChk(DAQmxWaitUntilTaskDone(task, timeToWait));
	DAQmxErrChk(DAQmxReadAnalogF64(task, -1, DAQmx_Val_WaitInfinitely, DAQmx_Val_GroupByScanNumber, readArray, sizeOfReadArray, &sampsPerChanRead, NULL));
//	for (int i = 0; i < sizeOfReadArray; ++i) {
//		printf("%.2lf ", readArray[i]);
//		if (i % 9 == 0 && i != 0) {
//			printf("\n");
//		}
//	}

	Error:
		if(DAQmxFailed(error))
			DAQmxGetExtendedErrorInfo(errorString, 2048);
		if(task != 0) {
			/*********************************************/
			// DAQmx Stop Code
			/*********************************************/
			DAQmxStopTask(task);
			DAQmxClearTask(task);
		}
		if(DAQmxFailed(error)) {
			printf("DAQmx Error: %s\n", errorString);
		}

		delete readArray;

	printf("\n%d samples per channel read supposedly\n", sampsPerChanRead);
	return 0;
}
