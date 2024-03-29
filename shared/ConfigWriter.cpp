#include <fstream>
#include <string>
#include <time.h>

#include "./ConfigWriter.h"



/* Example configuration file layout
 * recordingId=AJH
 * startTime=100
 * lowValue=-10.0
 * highValue=10.0
 * sampleRate=500000
 * timeToRead=30
 */

ConfigWriter::ConfigWriter(std::string fileName, unsigned int startTime,
        double expectedLowValue, double expectedHighValue,
        unsigned int sampleRate, unsigned int timeToRead, std::string recordingId)
        : configFile(fileName)
        , startTime(startTime)
        , expectedLowValue(expectedLowValue)
        , expectedHighValue(expectedHighValue)
        , sampleRate(sampleRate)
        , timeToRead(timeToRead)
        , recordingId(recordingId)
{
}


void ConfigWriter::write()
{
    std::ofstream fp(configFile.c_str());

    // Write file
    fp << "recordingId=" << recordingId << "\n";
    fp << "startTime=" << startTime << "\n";
    fp << "lowValue=" << expectedLowValue << "\n";
    fp << "highValue=" << expectedHighValue << "\n";
    fp << "sampleRate=" << sampleRate << "\n";
    fp << "timeToRead=" << timeToRead << "\n";

    fp.close();
}
