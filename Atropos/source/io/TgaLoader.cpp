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

    // Open the TGA file.
    FILE *filePtr = fopen(filename.c_str(), "rb");
    if (filePtr == NULL)
    {
        throw std::runtime_error("Failed to open TGA image: " + filename);
    }
    // Closing the file is tied to this scope so no throw below can leak it.
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

    // Which type of image gets stored in imageTypeCode.
    if (fread(&tgaFile->imageTypeCode, sizeof(unsigned char), 1, filePtr) != 1)
    {
        throw std::runtime_error("Failed to read TGA header: " + filename);
    }

    // For our purposes, the type code should be 2 (uncompressed RGB image)
    // or 3 (uncompressed black-and-white images).
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

    // Read the image's width and height.
    if (fread(&tgaFile->imageWidth, sizeof(short int), 1, filePtr) != 1
        || fread(&tgaFile->imageHeight, sizeof(short int), 1, filePtr) != 1)
    {
        throw std::runtime_error("Failed to read TGA dimensions: " + filename);
    }

    // Read the bit depth.
    if (fread(&tgaFile->bitCount, sizeof(unsigned char), 1, filePtr) != 1)
    {
        throw std::runtime_error("Failed to read TGA bit depth: " + filename);
    }

    // Read the image descriptor.
    if (fread(&tgaFile->imageDescriptor, sizeof(unsigned char), 1, filePtr) != 1)
    {
        throw std::runtime_error("Failed to read TGA header: " + filename);
    }

    // Color-mapped images are not supported.
    if (colorMapType != 0)
    {
        throw std::runtime_error("Color-mapped TGA images are not supported: " + filename);
    }

    // Skip the image identification field so the pixel data is read at the
    // correct offset.
    if (fseek(filePtr, idLength, SEEK_CUR) != 0)
    {
        throw std::runtime_error("Failed to read TGA image identification field: " + filename);
    }

    if (tgaFile->imageWidth <= 0 || tgaFile->imageHeight <= 0)
    {
        throw std::runtime_error("Invalid TGA image dimensions: " + filename);
    }

    // Only 24 and 32 bit images are supported, which is 3 or 4 bytes per pixel.
    // The bit depth itself has to be checked, a quotient would also accept
    // depths such as 26 or 33 and read them with the wrong stride.
    // Only 24 and 32 bit images are supported, which is 3 or 4 bytes per pixel.
    // The bit depth itself has to be checked, a quotient would also accept
    // depths such as 26 or 33 and read them with the wrong stride.
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

    // The pixel data starts at the current position, so the bytes that are left
    // in the file bound the size the header is allowed to declare. Without this
    // a header alone could make the loader allocate for gigabytes of pixels.
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

    // Allocate memory for the image data. It is owned by this scope until the
    // conversion is complete, so a throw cannot leak it.
    std::unique_ptr<unsigned char[]> imageData(new unsigned char[imageSize]());

    // Read the image data.
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

    // Bit 5 of the image descriptor is clear for a bottom-left origin, which
    // means data row 0 is the bottom row of the image. The rows are reversed
    // here so the image is stored top row first.
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
