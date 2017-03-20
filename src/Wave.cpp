#include <fstream>
#include <iostream>
#include <limits>
#include <stdint.h>
#include <stdio.h>
#include <string>
#include "./Wave.h"


// Helper function to write numbers in binary to file
template <typename T>
void writeNumberBinary(FILE *fp, T number, int numberOfElements=1)
{
	fwrite(&number, sizeof(T), numberOfElements, fp);
}


// Helper function to write character array that has no null character
void writeCharArrayWithoutANull(FILE *fp, const char *array, const unsigned int sizeOfArray)
{
	fwrite(array, sizeof(char), sizeOfArray, fp);
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
	FILE *fp = fopen(fileName.c_str(), "wb");

	// Write top level header
	writeCharArrayWithoutANull(fp, chunkId, 4);
	writeNumberBinary<uint32_t>(fp, chunkSize);
	writeCharArrayWithoutANull(fp, format, 4);

	// Write format chunk
	writeCharArrayWithoutANull(fp, fmtSubChunkId, 4);
	writeNumberBinary<uint32_t>(fp, fmtSubChunkSize);
	writeNumberBinary<uint16_t>(fp, fmtTag);
	writeNumberBinary<uint16_t>(fp, channels);
	writeNumberBinary<uint32_t>(fp, sampsPerSecond);
	writeNumberBinary<uint32_t>(fp, avgBytesPerSecond);
	writeNumberBinary<uint16_t>(fp, blockAlign);
	writeNumberBinary<uint16_t>(fp, bitsPerSamp);

	// Write data chunk
	writeCharArrayWithoutANull(fp, dataSubChunkId, 4);
	writeNumberBinary<uint32_t>(fp, dataSubChunkSize);
	std::size_t written = fwrite(data, sizeof(*data), numberOfDataElements, fp);
	if (written != numberOfDataElements)
	{
		std::cout << "Did not write all of the data" << std::endl;
	}

	// Check size of file and compare to chunkSize for sanity
	fflush(fp);  // Try to ensure the stream has been updated
	unsigned int endPosition = ftell(fp);
	if (chunkSize != (endPosition - 8))
	{
		std::cout << "The wav file's written chunkSize is: " << chunkSize
				  << "\nBut the file pointer claims it should be: " << endPosition - 8 << std::endl;
	}

	fclose(fp);
}
