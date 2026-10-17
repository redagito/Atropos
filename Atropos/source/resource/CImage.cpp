#include "resource/CImage.h"

#include "io/lodepng/lodepng.h"
#include "io/TgaLoader.h"
#include "debug/Log.h"

#include <cassert>

CImage::CImage()
	:
	d_width(0),
	d_height(0)
{
	return;
}

bool CImage::load(const std::string& fileName)
{
	// File extension
	auto pos = fileName.find_last_of(".");
	if (pos == std::string::npos)
	{
		return false;
	}
	std::string ext = fileName.substr(pos + 1);
	if (ext == "png")
	{
		return (lodepng::decode(d_data, d_width, d_height, fileName) == 0);
	}
	else if (ext == "tga")
	{
		LOG_WARNING("Deprecated tga file format used for file %s, convert to png for better performance and storage space.", fileName.c_str());
		TGAFILE file;
		if (!loadTGAFile(fileName, &file))
		{
			LOG_ERROR("Failed to load image file %s.", fileName.c_str());
			return false;
		}
		// Assign to vector
		// RGBA data
		int channels = file.bitCount / 8;
		if (channels == 4)
		{
			d_data.assign(file.imageData, file.imageData + file.imageWidth * file.imageHeight * 4);
		}
		else if (channels == 3)
		{
			d_data.clear();
			d_data.reserve(file.imageWidth * file.imageHeight * 4);
			for (int i = 0; i < file.imageWidth * file.imageHeight; ++i)
			{
				d_data.push_back(file.imageData[i * 3]);
				d_data.push_back(file.imageData[i * 3 + 1]);
				d_data.push_back(file.imageData[i * 3 + 2]);
				d_data.push_back(255);
			}
		}
		else
		{
			LOG_ERROR("Unsupported TGA color depth %d in file %s.", channels, fileName.c_str());
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

unsigned int CImage::getWidth() const
{
	return d_width;
}

unsigned int CImage::getHeight() const
{
	return d_height;
}

const unsigned char* CImage::getData() const
{
	return d_data.data();
}
