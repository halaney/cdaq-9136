#include <fstream>
#include <iostream>
#include <limits>
#include <stdint.h>
#include <stdio.h>
#include <string>
#include "./Wave.h"


// Helper function to write data in binary to file
template <typename T>
std::size_t writeDataBinary(FILE *fp, const T *data, unsigned int numberOfElements=1)
{
	return fwrite(data, sizeof(T), numberOfElements, fp);
}


WaveFile::WaveFile(const int16_t * const data, unsigned int numberOfDataElements, uint32_t sampsPerSecond)
				: fmtSubChunkSize(16)
				, fmtTag(1)
				, channels(1)
				, sampsPerSecond(sampsPerSecond)
				, bitsPerSamp(sizeof(int16_t) * 8)
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
	// Using c-style file I/O because c++ style did not like writing large blocks of a data
	FILE *fp = fopen(fileName.c_str(), "wb");

	// Write top level header
	writeDataBinary<char>(fp, chunkId, 4);
	writeDataBinary<uint32_t>(fp, &chunkSize);
	writeDataBinary<char>(fp, format, 4);

	// Write format chunk
	writeDataBinary<char>(fp, fmtSubChunkId, 4);
	writeDataBinary<uint32_t>(fp, &fmtSubChunkSize);
	writeDataBinary<uint16_t>(fp, &fmtTag);
	writeDataBinary<uint16_t>(fp, &channels);
	writeDataBinary<uint32_t>(fp, &sampsPerSecond);
	writeDataBinary<uint32_t>(fp, &avgBytesPerSecond);
	writeDataBinary<uint16_t>(fp, &blockAlign);
	writeDataBinary<uint16_t>(fp, &bitsPerSamp);

	// Write data chunk
	writeDataBinary<char>(fp, dataSubChunkId, 4);
	writeDataBinary<uint32_t>(fp, &dataSubChunkSize);
	std::size_t written = writeDataBinary<int16_t>(fp, data, numberOfDataElements);
	if (written != numberOfDataElements)
	{
		std::cout << "Did not write all of the data" << std::endl;
	}

	// Check size of file and compare to chunkSize for sanity
	fflush(fp);
	unsigned int endPosition = ftell(fp);
	if (chunkSize != (endPosition - 8))
	{
		std::cout << "The wav file's written chunkSize is: " << chunkSize
				  << "\nBut the file pointer claims it should be: " << endPosition - 8 << std::endl;
	}

	fclose(fp);
}
