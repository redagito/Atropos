#pragma once

#include <memory>
#include <string>
#include <vector>

/**
* \brief TGA image structure
*/
typedef struct
{
    unsigned char imageTypeCode;
    short int imageWidth;
    short int imageHeight;
    unsigned char bitCount;
    unsigned char imageDescriptor;
    std::unique_ptr<unsigned char[]> imageData; /**< Owned RGB pixel data. */
} TGAFILE;

/**
* \brief TGA loader function
*
* Throws std::runtime_error if the file cannot be loaded. On a throw the pixel
* data of tgaFile is left empty, so nothing has to be released by the caller.
*/
void loadTGAFile(const std::string& fileName, TGAFILE *tgaFile);