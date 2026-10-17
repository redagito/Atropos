#include "sound/AudioDecoder.h"

#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <string>
#include <vector>

namespace
{
	void appendU16(std::vector<char>& out, std::uint16_t value)
	{
		out.push_back(static_cast<char>(value & 0xFF));
		out.push_back(static_cast<char>((value >> 8) & 0xFF));
	}

	void appendU32(std::vector<char>& out, std::uint32_t value)
	{
		out.push_back(static_cast<char>(value & 0xFF));
		out.push_back(static_cast<char>((value >> 8) & 0xFF));
		out.push_back(static_cast<char>((value >> 16) & 0xFF));
		out.push_back(static_cast<char>((value >> 24) & 0xFF));
	}

	void appendTag(std::vector<char>& out, const char* tag)
	{
		out.insert(out.end(), tag, tag + 4);
	}

	std::vector<char> makeWav(int channels, int sampleRate, int bitsPerSample, const std::vector<short>& samples)
	{
		std::uint32_t dataSize = static_cast<std::uint32_t>(samples.size() * (bitsPerSample / 8));

		std::vector<char> out;
		appendTag(out, "RIFF");
		appendU32(out, 36 + dataSize);
		appendTag(out, "WAVE");
		appendTag(out, "fmt ");
		appendU32(out, 16);
		appendU16(out, 1);
		appendU16(out, static_cast<std::uint16_t>(channels));
		appendU32(out, static_cast<std::uint32_t>(sampleRate));
		appendU32(out, static_cast<std::uint32_t>(sampleRate * channels * (bitsPerSample / 8)));
		appendU16(out, static_cast<std::uint16_t>(channels * (bitsPerSample / 8)));
		appendU16(out, static_cast<std::uint16_t>(bitsPerSample));
		appendTag(out, "data");
		appendU32(out, dataSize);
		for (short sample : samples)
		{
			if (bitsPerSample == 8)
			{
				out.push_back(static_cast<char>(sample / 256 + 128));
			}
			else
			{
				appendU16(out, static_cast<std::uint16_t>(sample));
			}
		}
		return out;
	}
}

TEST_CASE("AudioDecoder decodes 16-bit PCM WAV data", "[audio]")
{
	const std::vector<short> samples = { 0, 1000, -1000, 32767, -32768, 42 };
	const std::vector<char> wav = makeWav(2, 44100, 16, samples);

	AudioDecoder::WaveData wave;
	REQUIRE(AudioDecoder::decode(wav, wave));
	CHECK(wave.channels == 2);
	CHECK(wave.sampleRate == 44100);
	REQUIRE(wave.samples.size() == samples.size());
	CHECK(wave.samples[0] == 0);
	CHECK(wave.samples[3] == 32767);
	CHECK(wave.samples[4] == -32768);
}

TEST_CASE("AudioDecoder decodes 8-bit PCM WAV data", "[audio]")
{
	const std::vector<short> samples = { 0, 256, -256, 32512, -32768 };
	const std::vector<char> wav = makeWav(1, 16000, 8, samples);

	AudioDecoder::WaveData wave;
	REQUIRE(AudioDecoder::decode(wav, wave));
	CHECK(wave.channels == 1);
	CHECK(wave.sampleRate == 16000);
	REQUIRE(wave.samples.size() == samples.size());
	CHECK(wave.samples == samples);
}

TEST_CASE("AudioDecoder clamps an oversized WAV data chunk", "[audio]")
{
	const std::vector<short> samples = { 100, -100, 200 };
	std::vector<char> wav = makeWav(1, 16000, 16, samples);
	// Corrupt the data chunk size field to claim far more than the file holds
	const std::size_t dataSizeOffset = 40;
	wav[dataSizeOffset + 0] = static_cast<char>(0xFF);
	wav[dataSizeOffset + 1] = static_cast<char>(0xFF);
	wav[dataSizeOffset + 2] = static_cast<char>(0xFF);
	wav[dataSizeOffset + 3] = static_cast<char>(0xFF);

	AudioDecoder::WaveData wave;
	REQUIRE(AudioDecoder::decode(wav, wave));
	CHECK(wave.samples == samples);
}

TEST_CASE("AudioDecoder rejects data that is not audio", "[audio]")
{
	const std::vector<char> garbage = { 'n', 'o', 't', 'a', 'u', 'd', 'i', 'o' };

	AudioDecoder::WaveData wave;
	CHECK_FALSE(AudioDecoder::decode(garbage, wave));
}

TEST_CASE("AudioDecoder reports missing files", "[audio]")
{
	AudioDecoder::WaveData wave;
	CHECK_FALSE(AudioDecoder::decodeFile("definitely_missing.mp3", wave));
}

TEST_CASE("AudioDecoder decodes an MP3 file", "[audio]")
{
	const std::string file = std::string(ATROPOS_SOUND_DIR) + "/sfx/laser2.mp3";

	AudioDecoder::WaveData wave;
	REQUIRE(AudioDecoder::decodeFile(file, wave));
	CHECK((wave.channels == 1 || wave.channels == 2));
	CHECK(wave.sampleRate > 0);
	REQUIRE_FALSE(wave.samples.empty());
	CHECK(wave.samples.size() % wave.channels == 0);
}

TEST_CASE("AudioDecoder decodes the 8-bit WAV sound effect", "[audio]")
{
	const std::string file = std::string(ATROPOS_SOUND_DIR) + "/sfx/explosion2.wav";

	AudioDecoder::WaveData wave;
	REQUIRE(AudioDecoder::decodeFile(file, wave));
	CHECK(wave.channels == 1);
	CHECK(wave.sampleRate == 16000);
	REQUIRE_FALSE(wave.samples.empty());
}
