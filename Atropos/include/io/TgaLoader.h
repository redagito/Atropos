#pragma once

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
    unsigned char *imageData;
} TGAFILE;

/**
* \brief TGA loader function
*
* Throws std::runtime_error if the file cannot be loaded.
*/
void loadTGAFile(const std::string& fileName, TGAFILE *tgaFile);
void deleteTGAFile(TGAFILE* tgaFile);