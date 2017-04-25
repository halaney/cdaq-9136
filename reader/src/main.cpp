#include <fstream>
#include <iostream>
#include <NIDAQmx.h>
#include <sstream>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "../../shared/ConfigReader.h"
#include "../../shared/ConfigWriter.h"
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


int main(int argc, char *argv[])
{
    // Create the non-configurable variables for NIDAQmx API
    TaskHandle task;
    const unsigned int numberOfChannels = 8;
    const char *physicalChannelNames = "cDAQ1Mod1/ai0:3, cDAQ1Mod2/ai0:3";  // "cDAQ1Mod1/ai0:3, cDAQ1Mod2/ai0:3" would use eight channels
    const char *taskName = "myTask";
    const int sizeOfErrorString = 2048;
    char errorString[sizeOfErrorString];
    int sampsPerChanRead;
    int16_t *readArray;

    // Create variables for tracking time and filenames
    time_t rawTime;
    struct tm *timeinfo;
    char fileNameBuffer[100];

    // Determine which configuration to use
    std::string configFile;
    if (argc == 1)
    {
        // Read user configuration file
        configFile = "/home/admin/reader/config/config.ini";
    }
    else if (argc == 2 &&
        (strcmp(argv[1], "--quick") == 0 || strcmp(argv[1], "-q") == 0))
    {
        // Read config for quick read
        configFile = "/home/admin/reader/config/quick-config.ini";
    }
    else
    {
        std::cout << "Invalid arguments" << std::endl;
        return 1;
    }
    ConfigReader reader(configFile);


    // Get configurable variables for NIDAQmx API and display values
    const double expectedLowValue = reader.getExpectedLowValue();
    const double expectedHighValue = reader.getExpectedHighValue();
    const unsigned int sampleRate = reader.getSampleRate();  // Sample rate in Hz per channel (NI-9223 has max sampling rate of 1 MHz)
    const unsigned int numberOfSecondsToRead = reader.getTimeToRead();
    const std::string recordingId = reader.getRecordingId();
    const unsigned int sampsPerChanToRead = sampleRate * numberOfSecondsToRead;
    unsigned int sizeOfReadArray = numberOfChannels * sampsPerChanToRead;
    unsigned int startTimeTemp;
    if (argc == 2)
    {
        // --quick option should start two minutes after usage
        // Update the config file to have the correct start time
        time(&rawTime);
        startTimeTemp = rawTime + 120;
        ConfigWriter writer(configFile, startTimeTemp, expectedLowValue,
            expectedHighValue, sampleRate, numberOfSecondsToRead, recordingId);
        writer.write();
        reader.reRead();
    }
    else
    {
        startTimeTemp = reader.getStartTime();
    }
    const unsigned int startTime = startTimeTemp;
    reader.display();  // If --quick option used the start time display is incorrect

    time(&rawTime);
    if ((startTime - rawTime - 5) > 0)
    {
        // Sleep until it is 5 seconds before startTime
        // This way we aren't holding resources and creating tasks without
        // properly closing them if the daemon is restarted
        std::cout << "Sleeping until start time..." << std::endl;
        sleep(startTime - rawTime - 5);
        std::cout << "Waking up..." << std::endl;
    }
    else
    {
        // If it's passed the start time don't bother
        std::cout << "Start time has already passed" << std::endl;
        return 0;
    }

    // 1GB of memory is close to the max amount this program can use
    // without changing the ADC buffer
    bool multipleReadsRequired = false;
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

    // Wait until the actual time to read
    time(&rawTime);
    if (startTime - rawTime > 0)
    {
        sleep(startTime - rawTime);
    }

    // Kick off the task and begin data acquisition
    time(&rawTime);  // Update time for output filenames
    DAQmxErrorCheck(DAQmxStartTask(task), errorString, sizeOfErrorString);

    // Read the data until all samples have been acquired
    unsigned int totalSampsPerChanRead = 0;
    while (totalSampsPerChanRead < sampsPerChanToRead)
    {
        timeinfo = localtime(&rawTime);
        strftime(fileNameBuffer, 100, "%Y%m%d_%H%M%S", timeinfo);

        // Perform the read from the ADC buffer
        DAQmxErrorCheck(DAQmxReadBinaryI16(task, -1, DAQmx_Val_WaitInfinitely, DAQmx_Val_GroupByChannel,
                readArray, sizeOfReadArray, &sampsPerChanRead, NULL), errorString, sizeOfErrorString);

        // Update time for next loop
        // indicating when exactly we stopped sampling for this file
        time(&rawTime);

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
        totalSampsPerChanRead += sampsPerChanRead;  // Increment counter
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
