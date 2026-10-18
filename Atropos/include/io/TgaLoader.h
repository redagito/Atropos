#pragma once

#include <memory>
#include <string>
#include <vector>

typedef struct
{
    unsigned char imageTypeCode;
    short int imageWidth;
    short int imageHeight;
    unsigned char bitCount;
    unsigned char imageDescriptor;
    std::unique_ptr<unsigned char[]> imageData; /**< Owned RGB pixel data. */
} TGAFILE;

/** \brief Throws std::runtime_error on failure; on a throw tgaFile holds no pixel data. */
void loadTGAFile(const std::string& fileName, TGAFILE *tgaFile);