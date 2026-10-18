#include "io/TgaLoader.h"

#include <cstddef>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>

/**
* \brief TGA loader function from http://stackoverflow.com/questions/7046270/loading-a-tga-file-and-using-it-with-opengl.
*/
void loadTGAFile(const std::string& filename, TGAFILE *tgaFile)
{
    unsigned char ucharBad;
    short int sintBad;

    FILE *filePtr = fopen(filename.c_str(), "rb");
    if (filePtr == NULL)
    {
        throw std::runtime_error("Failed to open TGA image: " + filename);
    }
    const std::unique_ptr<FILE, int (*)(FILE*)> fileGuard(filePtr, &fclose);

    tgaFile->imageData = nullptr;

    // Image identification field length and color map type.
    unsigned char idLength = 0;
    unsigned char colorMapType = 0;
    if (fread(&idLength, sizeof(unsigned char), 1, filePtr) != 1
        || fread(&colorMapType, sizeof(unsigned char), 1, filePtr) != 1)
    {
        throw std::runtime_error("Failed to read TGA header: " + filename);
    }

    if (fread(&tgaFile->imageTypeCode, sizeof(unsigned char), 1, filePtr) != 1)
    {
        throw std::runtime_error("Failed to read TGA header: " + filename);
    }

    // Only type 2 (uncompressed RGB) and 3 (uncompressed grayscale) are supported.
    if (tgaFile->imageTypeCode != 2 && tgaFile->imageTypeCode != 3)
    {
        throw std::runtime_error("Unsupported TGA image type: " + filename);
    }

    // Read 9 bytes of header data we don't need.
    if (fread(&sintBad, sizeof(short int), 1, filePtr) != 1
        || fread(&sintBad, sizeof(short int), 1, filePtr) != 1
        || fread(&ucharBad, sizeof(unsigned char), 1, filePtr) != 1
        || fread(&sintBad, sizeof(short int), 1, filePtr) != 1
        || fread(&sintBad, sizeof(short int), 1, filePtr) != 1)
    {
        throw std::runtime_error("Failed to read TGA header: " + filename);
    }

    if (fread(&tgaFile->imageWidth, sizeof(short int), 1, filePtr) != 1
        || fread(&tgaFile->imageHeight, sizeof(short int), 1, filePtr) != 1)
    {
        throw std::runtime_error("Failed to read TGA dimensions: " + filename);
    }

    if (fread(&tgaFile->bitCount, sizeof(unsigned char), 1, filePtr) != 1)
    {
        throw std::runtime_error("Failed to read TGA bit depth: " + filename);
    }

    if (fread(&tgaFile->imageDescriptor, sizeof(unsigned char), 1, filePtr) != 1)
    {
        throw std::runtime_error("Failed to read TGA header: " + filename);
    }

    // Color-mapped images are not supported.
    if (colorMapType != 0)
    {
        throw std::runtime_error("Color-mapped TGA images are not supported: " + filename);
    }

    // Skip the image identification field so pixel data starts at the right offset.
    if (fseek(filePtr, idLength, SEEK_CUR) != 0)
    {
        throw std::runtime_error("Failed to read TGA image identification field: " + filename);
    }

    if (tgaFile->imageWidth <= 0 || tgaFile->imageHeight <= 0)
    {
        throw std::runtime_error("Invalid TGA image dimensions: " + filename);
    }

    // Bit depth is checked directly; a quotient would also accept depths such
    // as 26 or 33 and read them with the wrong stride.
    if (tgaFile->bitCount != 24 && tgaFile->bitCount != 32)
    {
        throw std::runtime_error("Unsupported TGA color depth: " + filename);
    }
    const std::size_t colorMode = static_cast<std::size_t>(tgaFile->bitCount) / 8;
    if (colorMode < 3 || colorMode > 4)
    {
        throw std::runtime_error("Unsupported TGA color depth: " + filename);
    }

    // Size calculation in std::size_t avoids integer overflow.
    const std::size_t imageSize =
        static_cast<std::size_t>(tgaFile->imageWidth)
        * static_cast<std::size_t>(tgaFile->imageHeight)
        * colorMode;

    // The bytes left in the file bound what the header may declare, otherwise a
    // header alone could make the loader allocate for gigabytes of pixels.
    const long dataStart = ftell(filePtr);
    if (dataStart < 0)
    {
        throw std::runtime_error("Failed to read the TGA file position: " + filename);
    }
    if (fseek(filePtr, 0, SEEK_END) != 0)
    {
        throw std::runtime_error("Failed to seek to the end of the TGA file: " + filename);
    }
    const long fileSize = ftell(filePtr);
    if (fileSize < dataStart)
    {
        throw std::runtime_error("Failed to measure the TGA file: " + filename);
    }
    if (fseek(filePtr, dataStart, SEEK_SET) != 0)
    {
        throw std::runtime_error("Failed to seek to the TGA pixel data: " + filename);
    }
    if (imageSize > static_cast<std::size_t>(fileSize - dataStart))
    {
        throw std::runtime_error("TGA pixel data exceeds the file size: " + filename);
    }

    std::unique_ptr<unsigned char[]> imageData(new unsigned char[imageSize]());

    if (fread(imageData.get(), sizeof(unsigned char), imageSize, filePtr) != imageSize)
    {
        throw std::runtime_error("Failed to read TGA pixel data: " + filename);
    }

    // Change from BGR to RGB so OpenGL can read the image data.
    for (std::size_t imageIdx = 0; imageIdx < imageSize; imageIdx += colorMode)
    {
        unsigned char colorSwap = imageData[imageIdx];
        imageData[imageIdx] = imageData[imageIdx + 2];
        imageData[imageIdx + 2] = colorSwap;
    }

    // Descriptor bit 5 clear means a bottom-left origin, so data row 0 is the
    // bottom row; rows are reversed to store the image top row first.
    if ((tgaFile->imageDescriptor & 0x20) == 0)
    {
        const std::size_t rowSize = static_cast<std::size_t>(tgaFile->imageWidth) * colorMode;
        const std::size_t rowCount = imageSize / rowSize;
        for (std::size_t row = 0; row < rowCount / 2; ++row)
        {
            unsigned char* topRow = imageData.get() + row * rowSize;
            unsigned char* bottomRow = imageData.get() + (rowCount - 1 - row) * rowSize;
            for (std::size_t idx = 0; idx < rowSize; ++idx)
            {
                const unsigned char rowSwap = topRow[idx];
                topRow[idx] = bottomRow[idx];
                bottomRow[idx] = rowSwap;
            }
        }
    }

    tgaFile->imageData = std::move(imageData);
}
