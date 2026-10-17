#include "resource/Image.h"

#include "io/TgaLoader.h"

#include <spdlog/spdlog.h>

#include <stdexcept>

Image::Image()
	:
	d_width(0),
	d_height(0)
{
	return;
}

void Image::load(const std::string& fileName)
{
	// File extension
	const auto pos = fileName.find_last_of(".");
	if (pos == std::string::npos)
	{
		throw std::runtime_error("Image file has no extension: " + fileName);
	}
	const std::string ext = fileName.substr(pos + 1);
	if (ext != "tga")
	{
		throw std::runtime_error("Unsupported image format: " + fileName);
	}

	TGAFILE file;
	loadTGAFile(fileName, &file);

	// Assign to vector
	// RGBA data
	const int channels = file.bitCount / 8;
	const std::size_t pixelCount =
		static_cast<std::size_t>(file.imageWidth) * static_cast<std::size_t>(file.imageHeight);
	if (channels == 4)
	{
		d_data.assign(file.imageData.get(), file.imageData.get() + pixelCount * 4);
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
		throw std::runtime_error("Unsupported TGA color depth in file: " + fileName);
	}
	// Set size
	d_width = file.imageWidth;
	d_height = file.imageHeight;
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
