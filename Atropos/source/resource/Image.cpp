#include "resource/Image.h"

#include "io/TgaLoader.h"

#include <spdlog/spdlog.h>

#include <cassert>

Image::Image()
	:
	d_width(0),
	d_height(0)
{
	return;
}

bool Image::load(const std::string& fileName)
{
	// File extension
	auto pos = fileName.find_last_of(".");
	if (pos == std::string::npos)
	{
		return false;
	}
	std::string ext = fileName.substr(pos + 1);
	if (ext == "tga")
	{
		TGAFILE file;
		if (!loadTGAFile(fileName, &file))
		{
			SPDLOG_ERROR("Failed to load image file {}.", fileName);
			return false;
		}
		// Assign to vector
		// RGBA data
		const int channels = file.bitCount / 8;
		const std::size_t pixelCount =
			static_cast<std::size_t>(file.imageWidth) * static_cast<std::size_t>(file.imageHeight);
		if (channels == 4)
		{
			d_data.assign(file.imageData, file.imageData + pixelCount * 4);
		}
		else if (channels == 3)
		{
			d_data.clear();
			d_data.reserve(pixelCount * 4);
			for (std::size_t i = 0; i < pixelCount; ++i)
			{
				d_data.push_back(file.imageData[i * 3]);
				d_data.push_back(file.imageData[i * 3 + 1]);
				d_data.push_back(file.imageData[i * 3 + 2]);
				d_data.push_back(255);
			}
		}
		else
		{
			SPDLOG_ERROR("Unsupported TGA color depth {} in file {}.", channels, fileName);
			deleteTGAFile(&file);
			return false;
		}
		// Set size
		d_width = file.imageWidth;
		d_height = file.imageHeight;
		// Cleanup
		deleteTGAFile(&file);
		return true;
	}
	return false;
}

unsigned int Image::getWidth() const
{
	return d_width;
}

unsigned int Image::getHeight() const
{
	return d_height;
}

const unsigned char* Image::getData() const
{
	return d_data.data();
}
