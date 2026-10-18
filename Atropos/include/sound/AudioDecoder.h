#pragma once

#include <string>
#include <vector>

/**
* \brief Decodes audio files (WAV and MP3) into 16-bit PCM samples.
*/
class AudioDecoder
{
public:
	/**
	* \brief Decoded PCM audio, 16-bit signed and interleaved.
	*/
	struct WaveData
	{
		std::vector<short> samples;
		int channels;
		int sampleRate; /**< Sample rate in Hz. */
	};

	/**
	* \brief Decodes an audio file from disk.
	*/
	static bool decodeFile(const std::string& file, WaveData& out);

	/**
	* \brief Decodes an in-memory copy of a whole audio file.
	*/
	static bool decode(const std::vector<char>& data, WaveData& out);

private:
	static bool decodeWav(const std::vector<char>& data, WaveData& out);
	static bool decodeMp3(const std::vector<char>& data, WaveData& out);
};
