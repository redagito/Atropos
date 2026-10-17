#include "io/TgaLoader.h"

#include <cstddef>
#include <cstdio>
#include <stdexcept>
#include <string>

/**
* \brief TGA loader function from http://stackoverflow.com/questions/7046270/loading-a-tga-file-and-using-it-with-opengl.
*/
void loadTGAFile(const std::string& filename, TGAFILE *tgaFile)
{
    FILE *filePtr;
    unsigned char ucharBad;
    short int sintBad;

    // Open the TGA file.
    filePtr = fopen(filename.c_str(), "rb");
    if (filePtr == NULL)
    {
        throw std::runtime_error("Failed to open TGA image: " + filename);
    }

    tgaFile->imageData = nullptr;

    // Image identification field length and color map type.
    unsigned char idLength = 0;
    unsigned char colorMapType = 0;
    if (fread(&idLength, sizeof(unsigned char), 1, filePtr) != 1
        || fread(&colorMapType, sizeof(unsigned char), 1, filePtr) != 1)
    {
        fclose(filePtr);
        throw std::runtime_error("Failed to read TGA header: " + filename);
    }

    // Which type of image gets stored in imageTypeCode.
    if (fread(&tgaFile->imageTypeCode, sizeof(unsigned char), 1, filePtr) != 1)
    {
        fclose(filePtr);
        throw std::runtime_error("Failed to read TGA header: " + filename);
    }

    // For our purposes, the type code should be 2 (uncompressed RGB image)
    // or 3 (uncompressed black-and-white images).
    if (tgaFile->imageTypeCode != 2 && tgaFile->imageTypeCode != 3)
    {
        fclose(filePtr);
        throw std::runtime_error("Unsupported TGA image type: " + filename);
    }

    // Read 9 bytes of header data we don't need.
    if (fread(&sintBad, sizeof(short int), 1, filePtr) != 1
        || fread(&sintBad, sizeof(short int), 1, filePtr) != 1
        || fread(&ucharBad, sizeof(unsigned char), 1, filePtr) != 1
        || fread(&sintBad, sizeof(short int), 1, filePtr) != 1
        || fread(&sintBad, sizeof(short int), 1, filePtr) != 1)
    {
        fclose(filePtr);
        throw std::runtime_error("Failed to read TGA header: " + filename);
    }

    // Read the image's width and height.
    if (fread(&tgaFile->imageWidth, sizeof(short int), 1, filePtr) != 1
        || fread(&tgaFile->imageHeight, sizeof(short int), 1, filePtr) != 1)
    {
        fclose(filePtr);
        throw std::runtime_error("Failed to read TGA dimensions: " + filename);
    }

    // Read the bit depth.
    if (fread(&tgaFile->bitCount, sizeof(unsigned char), 1, filePtr) != 1)
    {
        fclose(filePtr);
        throw std::runtime_error("Failed to read TGA bit depth: " + filename);
    }

    // Read one byte of data we don't need.
    if (fread(&ucharBad, sizeof(unsigned char), 1, filePtr) != 1)
    {
        fclose(filePtr);
        throw std::runtime_error("Failed to read TGA header: " + filename);
    }

    // Color-mapped images are not supported.
    if (colorMapType != 0)
    {
        fclose(filePtr);
        throw std::runtime_error("Color-mapped TGA images are not supported: " + filename);
    }

    // Skip the image identification field so the pixel data is read at the
    // correct offset.
    if (fseek(filePtr, idLength, SEEK_CUR) != 0)
    {
        fclose(filePtr);
        throw std::runtime_error("Failed to read TGA image identification field: " + filename);
    }

    if (tgaFile->imageWidth <= 0 || tgaFile->imageHeight <= 0)
    {
        fclose(filePtr);
        throw std::runtime_error("Invalid TGA image dimensions: " + filename);
    }

    // Color mode -> 3 = BGR, 4 = BGRA.
    const std::size_t colorMode = static_cast<std::size_t>(tgaFile->bitCount) / 8;
    if (colorMode < 3 || colorMode > 4)
    {
        fclose(filePtr);
        throw std::runtime_error("Unsupported TGA color depth: " + filename);
    }

    // Size calculation in std::size_t avoids integer overflow.
    const std::size_t imageSize =
        static_cast<std::size_t>(tgaFile->imageWidth)
        * static_cast<std::size_t>(tgaFile->imageHeight)
        * colorMode;

    // Allocate memory for the image data.
    tgaFile->imageData = new unsigned char[imageSize]();

    // Read the image data.
    if (fread(tgaFile->imageData, sizeof(unsigned char), imageSize, filePtr) != imageSize)
    {
        delete[] tgaFile->imageData;
        tgaFile->imageData = nullptr;
        fclose(filePtr);
        throw std::runtime_error("Failed to read TGA pixel data: " + filename);
    }

    // Change from BGR to RGB so OpenGL can read the image data.
    for (std::size_t imageIdx = 0; imageIdx < imageSize; imageIdx += colorMode)
    {
        unsigned char colorSwap = tgaFile->imageData[imageIdx];
        tgaFile->imageData[imageIdx] = tgaFile->imageData[imageIdx + 2];
        tgaFile->imageData[imageIdx + 2] = colorSwap;
    }

    fclose(filePtr);
}

void deleteTGAFile(TGAFILE* tgaFile)
{
	if (tgaFile != nullptr && tgaFile->imageData != nullptr)
	{
		delete[] tgaFile->imageData;
		tgaFile->imageData = nullptr;
	}
}
