#include <fstream>
#include <iostream>
#include <stdint.h>
#include <string>
#include "./Wave.h"


// Helper function to write numbers in binary to file
void writeNumberBinary(std::fstream &fp, const int number)
{
	fp.write(reinterpret_cast<const char *>(&number), sizeof(number));
}


WaveFile::WaveFile(const int16_t * const data, unsigned int numberOfDataElements, uint32_t sampsPerSecond)
				: sampsPerSecond(sampsPerSecond)
				, data(data)
				, numberOfDataElements(numberOfDataElements)
{
	chunkId[0] = 'R';
	chunkId[1] = 'I';
	chunkId[2] = 'F';
	chunkId[3] = 'F';
	format[0] = 'W';
	format[1] = 'A';
	format[2] = 'V';
	format[3] = 'E';
	fmtSubChunkId[0] = 'f';
	fmtSubChunkId[1] = 'm';
	fmtSubChunkId[2] = 't';
	fmtSubChunkId[3] = ' ';
	dataSubChunkId[0] = 'd';
	dataSubChunkId[1] = 'a';
	dataSubChunkId[2] = 't';
	dataSubChunkId[3] = 'a';


	avgBytesPerSecond = (bitsPerSamp / 8) * sampsPerSecond;
	blockAlign = channels * (bitsPerSamp / 8);
	dataSubChunkSize = (bitsPerSamp / 8) * numberOfDataElements;

	// format + fmtChunk + dataChunkHeader + data
	chunkSize = 4 + 24 + 8 + (numberOfDataElements * (bitsPerSamp / 8));
}


void WaveFile::writeToFile(std::string fileName)
{
	std::fstream fp;
	fp.open(fileName.c_str(), std::ios::binary | std::fstream::out);

	//TODO: Determine why filepointer not to chunkSize, test wav file once fixed
	std::cout << "Beginning of file pointer location: " << fp.tellp() << std::endl;


	// Write top level header
	fp << chunkId;
	writeNumberBinary(fp, chunkSize);
	fp << format;

	// Write format chunk
	fp << fmtSubChunkId;
	writeNumberBinary(fp, fmtSubChunkSize);
	writeNumberBinary(fp, fmtTag);
	writeNumberBinary(fp, channels);
	writeNumberBinary(fp, sampsPerSecond);
	writeNumberBinary(fp, avgBytesPerSecond);
	writeNumberBinary(fp, blockAlign);
	writeNumberBinary(fp, bitsPerSamp);

	// Write data chunk
	fp << dataSubChunkId;
	writeNumberBinary(fp, dataSubChunkSize);
	for (unsigned int i = 0; i < numberOfDataElements; ++i)
	{
		writeNumberBinary(fp, *(data + i));
	}

	// Check size of file and compare to chunkSize for sanity
	fp.flush();  // Try to ensure the stream has been updated
	unsigned int endPosition = fp.tellp();
	if (chunkSize != (endPosition - 8))
	{
		std::cout << "The wav file's written chunkSize is: " << chunkSize
				  << "\nBut the file pointer claims it should be: " << endPosition - 8 << std::endl;
	}

	fp.close();
}
