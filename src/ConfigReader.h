#ifndef CONFIGREADER_H_
#define CONFIGREADER_H_
// This should really be replaced by some open-source json reader
#include <string>

class ConfigReader
{
public:
	ConfigReader(const std::string configFile);  // Reads config upon construction
	void reRead();  // Reread the config file
	unsigned int getStartTime() const;  // Gets the time to start reading from the sensor
	int getExpectedLowValue() const;  // Gets the expected lowest value the sensor should see
	int getExpectedHighValue() const;  // Gets the expected highest value sensor should see
	unsigned int getSampleRate() const;  // Gets the sample rate in Hz
	unsigned int getTimeToRead() const;  // Gets how long the sensor should read

private:
	void read();
	const std::string configFile;
	unsigned int startTime;
	int expectedLowValue;
	int expectedHighValue;
	unsigned int sampleRate;
	unsigned int timeToRead;
};

#endif /* CONFIGREADER_H_ */
