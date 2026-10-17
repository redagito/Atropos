#include "sound/CAudioDecoder.h"

#define MINIMP3_IMPLEMENTATION
#include <minimp3_ex.h>

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>

namespace
{
	std::uint16_t readU16(const std::vector<char>& data, std::size_t offset)
	{
		return static_cast<std::uint16_t>(static_cast<unsigned char>(data[offset]))
			| (static_cast<std::uint16_t>(static_cast<unsigned char>(data[offset + 1])) << 8);
	}

	std::uint32_t readU32(const std::vector<char>& data, std::size_t offset)
	{
		return static_cast<std::uint32_t>(static_cast<unsigned char>(data[offset]))
			| (static_cast<std::uint32_t>(static_cast<unsigned char>(data[offset + 1])) << 8)
			| (static_cast<std::uint32_t>(static_cast<unsigned char>(data[offset + 2])) << 16)
			| (static_cast<std::uint32_t>(static_cast<unsigned char>(data[offset + 3])) << 24);
	}
}

bool CAudioDecoder::decodeFile(const std::string& file, SWaveData& out)
{
	std::ifstream stream(file, std::ios::binary | std::ios::ate);
	if (!stream.is_open())
	{
		return false;
	}

	std::streamsize size = stream.tellg();
	if (size <= 0)
	{
		return false;
	}

	std::vector<char> data(static_cast<std::size_t>(size));
	stream.seekg(0, std::ios::beg);
	if (!stream.read(data.data(), size))
	{
		return false;
	}

	return decode(data, out);
}

bool CAudioDecoder::decode(const std::vector<char>& data, SWaveData& out)
{
	if (data.size() >= 12
		&& std::memcmp(data.data(), "RIFF", 4) == 0
		&& std::memcmp(data.data() + 8, "WAVE", 4) == 0)
	{
		return decodeWav(data, out);
	}

	return decodeMp3(data, out);
}

bool CAudioDecoder::decodeWav(const std::vector<char>& data, SWaveData& out)
{
	std::size_t pos = 12;
	bool haveFormat = false;
	std::uint16_t audioFormat = 0;
	int channels = 0;
	int sampleRate = 0;
	int bitsPerSample = 0;
	const char* dataChunk = nullptr;
	std::size_t dataSize = 0;

	while (pos + 8 <= data.size())
	{
		const char* id = data.data() + pos;
		std::uint32_t chunkSize = readU32(data, pos + 4);
		std::size_t body = pos + 8;

		if (std::memcmp(id, "fmt ", 4) == 0 && body + 16 <= data.size())
		{
			audioFormat = readU16(data, body);
			channels = readU16(data, body + 2);
			sampleRate = static_cast<int>(readU32(data, body + 4));
			bitsPerSample = readU16(data, body + 14);
			haveFormat = true;
		}
		else if (std::memcmp(id, "data", 4) == 0)
		{
			dataChunk = data.data() + body;
			dataSize = chunkSize;
			if (body + dataSize > data.size())
			{
				dataSize = data.size() - body;
			}
		}

		pos = body + chunkSize + (chunkSize & 1);
	}

	if (!haveFormat || dataChunk == nullptr || audioFormat != 1
		|| (bitsPerSample != 8 && bitsPerSample != 16)
		|| channels <= 0 || sampleRate <= 0)
	{
		return false;
	}

	std::size_t bytesPerSample = static_cast<std::size_t>(bitsPerSample / 8);
	if (dataSize < bytesPerSample)
	{
		return false;
	}

	std::size_t sampleCount = dataSize / bytesPerSample;
	out.samples.resize(sampleCount);
	if (bitsPerSample == 8)
	{
		const unsigned char* source = reinterpret_cast<const unsigned char*>(dataChunk);
		for (std::size_t i = 0; i < sampleCount; ++i)
		{
			out.samples[i] = static_cast<short>((static_cast<int>(source[i]) - 128) * 256);
		}
	}
	else
	{
		std::memcpy(out.samples.data(), dataChunk, sampleCount * sizeof(short));
	}
	out.channels = channels;
	out.sampleRate = sampleRate;
	return true;
}

bool CAudioDecoder::decodeMp3(const std::vector<char>& data, SWaveData& out)
{
	if (data.empty())
	{
		return false;
	}

	mp3dec_t decoder;
	mp3dec_init(&decoder);

	mp3dec_file_info_t info;
	std::memset(&info, 0, sizeof(info));
	if (mp3dec_load_buf(&decoder, reinterpret_cast<const std::uint8_t*>(data.data()), data.size(), &info, nullptr, nullptr) != 0)
	{
		return false;
	}

	if (info.samples == 0 || info.channels <= 0)
	{
		std::free(info.buffer);
		return false;
	}

	out.samples.assign(info.buffer, info.buffer + info.samples);
	out.channels = info.channels;
	out.sampleRate = info.hz;
	std::free(info.buffer);
	return true;
}
