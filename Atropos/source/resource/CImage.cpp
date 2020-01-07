#include "CImage.h"

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
		d_data.assign(file.imageData, file.imageData + file.imageWidth * file.imageHeight * 4);
		// Set size
		d_width = file.imageWidth;
		d_height = file.imageHeight;
		// Cleanup
		deleteTGAFile(&file);
		return true;
	}
	return false;
}

void CImage::init(unsigned int width, unsigned int height, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	d_width = width;
	d_height = height;
	d_data.resize(d_width * d_height * 4);
	for (unsigned int i = 0; i < d_data.size(); i+= 4)
	{
		d_data[i] = r;
		d_data[i] = g;
		d_data[i] = b;
		d_data[i] = a;
	}
}

void CImage::init(unsigned int width, unsigned int height, unsigned char* data)
{
	assert(data != nullptr);
	d_width = width;
	d_height = height;
	d_data.resize(d_width * d_height * 4);
	d_data.assign(data, data + d_width * d_height * 4);
}

void CImage::set(unsigned int width, unsigned int height, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	assert(width < d_width);
	assert(height < d_height);
	width %= d_width;
	height %= d_height;
	d_data[(width + d_width * height) * 4] = r;
	d_data[(width + d_width * height) * 4 + 1] = g;
	d_data[(width + d_width * height) * 4 + 2] = b;
	d_data[(width + d_width * height) * 4 + 3] = a;
}

bool CImage::save(const std::string& fileName) const
{
	return (lodepng::encode(fileName, d_data, d_width, d_height) == 0);
}

unsigned char CImage::getRed(unsigned int width, unsigned int height) const
{
	assert(width < d_width);
	assert(height < d_height);
	return d_data.at((width + d_width * height) * 4);
}

unsigned char CImage::getGreen(unsigned int width, unsigned int height) const
{
	assert(width < d_width);
	assert(height < d_height);
	return d_data.at((width + d_width * height) * 4 + 1);
}

unsigned char CImage::getBlue(unsigned int width, unsigned int height) const
{
	assert(width < d_width);
	assert(height < d_height);
	return d_data.at((width + d_width * height) * 4 + 2);
}

unsigned char CImage::getAlpha(unsigned int width, unsigned int height) const
{
	assert(width < d_width);
	assert(height < d_height);
	return d_data.at((width + d_width * height) * 4 + 3);
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
