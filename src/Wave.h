#ifndef WAVE_H_
#define WAVE_H_

#include <stdint.h>
#include <string>


// A class for creating a wav file (PCM format, single channel,
// currently 16 bit sample size). This only works on little-endian systems
// because of the little-endian nature of a wav file.
class WaveFile
{
public:
	// Initializes variables to be written to the wav file
	// @data: a pointer to the data to be written (not enough memory to copy)
	// @numberOfDataElements: Number of elements from @data to be written
	// @sapsPerSecond: The sampling rate of the data in Hz
	WaveFile(const int16_t * const data, unsigned int numberOfDataElements, uint32_t sampsPerSecond);

	// Writes the wav file
	// @fileName: the filename to write the file to
	void writeToFile(std::string fileName);

private:
	/* Top level chunk */
	char chunkId[4];  // Specifies chunk ID "RIFF"
	uint32_t chunkSize;  // Total size of file excluding 8 bytes from RIFF & chunkSize
	char format[4];  // Specifies riff-wave type "WAVE"

	/* Format subchunk */
	char fmtSubChunkId[4];  // Names subchunk ID "fmt "
	static const uint32_t fmtSubChunkSize = 16;  // Size in bytes of this chunk
	static const uint16_t fmtTag = 1;  // Specifies PCM wav filetype
	static const uint16_t channels = 1;  // Mono channel wav file
	uint32_t sampsPerSecond;  // Sampling rate
	uint32_t avgBytesPerSecond;  // Bytes per second
	uint16_t blockAlign;  // Bytes in a sound block (i.e. channels * bitsPerSamp / 8)
	static const uint16_t bitsPerSamp = sizeof(int16_t) * 8;

	/* Data subchunk */
	char dataSubChunkId[4];  // Names subchunk ID "data"
	uint32_t dataSubChunkSize;  // Size of actual data put in file
	const int16_t * const data;  // Pointer to the data to be written to file... Not enough memory to copy

	/* Miscellaneous info not part of wave file */
	unsigned int numberOfDataElements;
};

#endif /* WAVE_H_ */
