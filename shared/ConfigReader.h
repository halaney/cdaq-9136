#ifndef CONFIGREADER_H_
#define CONFIGREADER_H_
// This should really be replaced by some open-source .ini reader
#include <string>

class ConfigReader
{
public:
    ConfigReader(const std::string configFile);  // Reads config upon construction
    void reRead();  // Reread the config file
    void display();  // Displays current parameters
    unsigned int getStartTime() const;  // Gets the time to start reading from the sensor
    double getExpectedLowValue() const;  // Gets the expected lowest value the sensor should see
    double getExpectedHighValue() const;  // Gets the expected highest value sensor should see
    unsigned int getSampleRate() const;  // Gets the sample rate in Hz
    unsigned int getTimeToRead() const;  // Gets how long the sensor should read
    std::string getRecordingId() const;  // Gets the user specified recording ID

private:
    void read();
    const std::string configFile;
    unsigned int startTime;
    double expectedLowValue;
    double expectedHighValue;
    unsigned int sampleRate;
    unsigned int timeToRead;
    std::string recordingId;
};

#endif /* CONFIGREADER_H_ */
