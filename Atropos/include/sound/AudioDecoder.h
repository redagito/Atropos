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
		std::vector<short> samples; /**< Interleaved 16-bit samples. */
		int channels; /**< Number of channels. */
		int sampleRate; /**< Sample rate in Hz. */
	};

	/**
	* \brief Decodes an audio file from disk.
	* \param file Path to the audio file.
	* \param out Receives the decoded PCM data.
	* \return True on success.
	*/
	static bool decodeFile(const std::string& file, WaveData& out);

	/**
	* \brief Decodes audio data held in memory.
	* \param data Raw file contents.
	* \param out Receives the decoded PCM data.
	* \return True on success.
	*/
	static bool decode(const std::vector<char>& data, WaveData& out);

private:
	static bool decodeWav(const std::vector<char>& data, WaveData& out);
	static bool decodeMp3(const std::vector<char>& data, WaveData& out);
};
