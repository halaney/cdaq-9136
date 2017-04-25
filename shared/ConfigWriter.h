#ifndef CONFIGWRITER_H_
#define CONFIGWRITER_H_

#include <string>


class ConfigWriter
{
public:
    ConfigWriter(std::string fileName, unsigned int startTime,
            double expectedLowValue, double expectedHighValue,
            unsigned int sampleRate, unsigned int timeToRead,
            std::string recordingId);
    void write();  // Writes the config to the file

private:
    const std::string configFile;
    unsigned int startTime;
    double expectedLowValue;
    double expectedHighValue;
    unsigned int sampleRate;
    unsigned int timeToRead;
    std::string recordingId;
};

#endif /* CONFIGWRITER_H_ */
