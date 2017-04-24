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
    const std::string recordingId = reader.getRecordingId();
    const unsigned int sampsPerChanToRead = sampleRate * numberOfSecondsToRead;
    unsigned int sizeOfReadArray = numberOfChannels * sampsPerChanToRead;
    reader.display();

    // Wait till it is 5 seconds before startTime
    // This way we aren't holding resources and creating tasks without properly closing them if the daemon is restarted
    time_t rawTime;
    time(&rawTime);
    if ((startTime - rawTime - 5) > 0)
    {
        std::cout << "Sleeping until start time..." << std::endl;
        sleep(startTime - rawTime - 5);
        std::cout << "Waking up..." << std::endl;
    }
    else  // If it is passed the start time don't bother
    {
        std::cout << "Start time has already passed" << std::endl;
        return 0;
    }
    // 1GB of memory is close to the max amount this program can use
    // without changing the ADC buffer
    bool multipleReadsRequired = false;
    int16_t *readArray;
    if (sizeOfReadArray > 400000000)
    {
        // We have to split the acquisition into multiple reads
        // The internal ADC buffer will use as much memory as needed to hold
        // all the samples. We will have to configure the buffer ourselves
        // to hold less since the system only has 2 GB of memory
        multipleReadsRequired = true;
        sizeOfReadArray = 400000000;  // 0.8 GB is a safe max
        readArray = new int16_t[sizeOfReadArray];
    }
    else
    {
        // The sample size is small enough we can read it all at once
        readArray = new int16_t[sizeOfReadArray];
    }

    // Create a "task" for this data acquisition
    DAQmxErrorCheck(DAQmxCreateTask(taskName, &task), errorString, sizeOfErrorString);

    // Create and add virtual channels to the task to indicate where to read data from
    DAQmxErrorCheck(DAQmxCreateAIVoltageChan(task, physicalChannelNames, "", DAQmx_Val_Diff,
            expectedLowValue, expectedHighValue, DAQmx_Val_Volts, NULL), errorString, sizeOfErrorString);

    // Configure timing parameters for reading
    DAQmxErrorCheck(DAQmxCfgSampClkTiming(task, NULL, sampleRate, DAQmx_Val_Rising,
            DAQmx_Val_FiniteSamps, sampsPerChanToRead), errorString, sizeOfErrorString);

    // Limit the ADC buffer to (400000000 / 8) samples per channel
    // so we don't run out of memory doing multiple reads
    if (multipleReadsRequired)
    {
        DAQmxErrorCheck(DAQmxCfgInputBuffer(task, 400000000 / 8),
            errorString, sizeOfErrorString);
    }

    // Wait till actual time to read
    time(&rawTime);
    if (startTime - rawTime > 0)
    {
        sleep(startTime - rawTime);
    }

    // Kick off the task
    DAQmxErrorCheck(DAQmxStartTask(task), errorString, sizeOfErrorString);

    unsigned int totalSampsPerChanRead = 0;
    while (totalSampsPerChanRead < sampsPerChanToRead)
    {
        struct tm *timeinfo;
        char fileNameBuffer[100];
        time(&rawTime);
        timeinfo = localtime(&rawTime);
        strftime(fileNameBuffer, 100, "%Y%m%d_%H%M%S", timeinfo);

        // Perform the read from the ADC buffer
        DAQmxErrorCheck(DAQmxReadBinaryI16(task, -1, DAQmx_Val_WaitInfinitely, DAQmx_Val_GroupByChannel,
                readArray, sizeOfReadArray, &sampsPerChanRead, NULL), errorString, sizeOfErrorString);

        // Write data out to file
        for (unsigned int i = 0; i < numberOfChannels; ++i)
        {
            std::stringstream ss;
            ss << "/home/admin/reader/output/" << recordingId
               << "_" << fileNameBuffer << "CH0" << i + 1 << ".wav";
            std::string fileName = ss.str();
            WaveFile fp(readArray + (i * sampsPerChanRead), sampsPerChanRead, sampleRate);
            std::cout << "Writing: " << fileName << std::endl;
            fp.writeToFile(fileName);
        }
        totalSampsPerChanRead += sampsPerChanRead;
    }

    // Clear the task out if it still exists
    if (task != NULL)
    {
        DAQmxStopTask(task);
        DAQmxClearTask(task);
    }

    delete readArray;
    return 0;
}
