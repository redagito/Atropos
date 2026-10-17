#include "resource/Image.h"

#include <catch2/catch_test_macros.hpp>

#include <process.h>

#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace
{
	std::filesystem::path tempImagePath()
	{
		// ctest gives every test case its own process, so the counter restarts
		// at zero everywhere and would collide across concurrent runs. The
		// process id makes the name unique among simultaneously running tests.
		static int counter = 0;
		return std::filesystem::temp_directory_path()
			/ ("atropos_img_" + std::to_string(_getpid()) + "_" + std::to_string(counter++) + ".tga");
	}

	void writeFile(const std::filesystem::path& path, const std::vector<unsigned char>& bytes)
	{
		std::ofstream out(path, std::ios::binary);
		out.write(reinterpret_cast<const char*>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
	}
}

TEST_CASE("Image loads a 24-bit TGA as RGBA", "[image]")
{
	const std::vector<unsigned char> header = {
		0, 0, 2,
		0, 0, 0, 0, 0,
		0, 0,
		0, 0,
		2, 0,
		1, 0,
		24,
		0
	};
	const std::vector<unsigned char> pixels = { 10, 20, 30, 40, 50, 60 };

	std::vector<unsigned char> bytes = header;
	bytes.insert(bytes.end(), pixels.begin(), pixels.end());

	const std::filesystem::path path = tempImagePath();
	writeFile(path, bytes);

	Image image;
	REQUIRE_NOTHROW(image.load(path.string()));
	std::filesystem::remove(path);
	REQUIRE(image.getWidth() == 2);
	REQUIRE(image.getHeight() == 1);
	const unsigned char* data = image.getData();
	CHECK(data[0] == 30);
	CHECK(data[1] == 20);
	CHECK(data[2] == 10);
	CHECK(data[3] == 255);
	CHECK(data[4] == 60);
	CHECK(data[5] == 50);
	CHECK(data[6] == 40);
	CHECK(data[7] == 255);
}

TEST_CASE("Image skips the TGA image identification field", "[image]")
{
	// The image ID field is 4 bytes long and must be skipped before the
	// pixel data, otherwise every pixel is read at the wrong offset.
	const std::vector<unsigned char> header = {
		4, 0, 2,
		0, 0, 0, 0, 0,
		0, 0,
		0, 0,
		2, 0,
		1, 0,
		24,
		0
	};
	const std::vector<unsigned char> imageId = { 0xAA, 0xBB, 0xCC, 0xDD };
	const std::vector<unsigned char> pixels = { 10, 20, 30, 40, 50, 60 };

	std::vector<unsigned char> bytes = header;
	bytes.insert(bytes.end(), imageId.begin(), imageId.end());
	bytes.insert(bytes.end(), pixels.begin(), pixels.end());

	const std::filesystem::path path = tempImagePath();
	writeFile(path, bytes);

	Image image;
	REQUIRE_NOTHROW(image.load(path.string()));
	std::filesystem::remove(path);
	REQUIRE(image.getWidth() == 2);
	REQUIRE(image.getHeight() == 1);
	const unsigned char* data = image.getData();
	CHECK(data[0] == 30);
	CHECK(data[1] == 20);
	CHECK(data[2] == 10);
	CHECK(data[3] == 255);
	CHECK(data[4] == 60);
	CHECK(data[5] == 50);
	CHECK(data[6] == 40);
	CHECK(data[7] == 255);
}

TEST_CASE("Image orients a TGA by its origin bit", "[image]")
{
	// A 2x2 image whose first and last data row have clearly different colours.
	const std::vector<unsigned char> pixels = {
		10, 20, 30, 40, 50, 60,
		70, 80, 90, 100, 110, 120
	};
	const std::vector<unsigned char> bottomLeftHeader = {
		0, 0, 2,
		0, 0, 0, 0, 0,
		0, 0,
		0, 0,
		2, 0,
		2, 0,
		24,
		0
	};
	const std::vector<unsigned char> topLeftHeader = {
		0, 0, 2,
		0, 0, 0, 0, 0,
		0, 0,
		0, 0,
		2, 0,
		2, 0,
		24,
		32
	};

	// Bit 5 of the descriptor is clear, so data row 0 is the bottom row of
	// the image and the rows must end up reversed.
	std::vector<unsigned char> bottomLeftBytes = bottomLeftHeader;
	bottomLeftBytes.insert(bottomLeftBytes.end(), pixels.begin(), pixels.end());
	const std::filesystem::path bottomLeftPath = tempImagePath();
	writeFile(bottomLeftPath, bottomLeftBytes);

	Image bottomLeft;
	REQUIRE_NOTHROW(bottomLeft.load(bottomLeftPath.string()));
	std::filesystem::remove(bottomLeftPath);
	REQUIRE(bottomLeft.getHeight() == 2);
	const unsigned char* bottomLeftData = bottomLeft.getData();
	CHECK(bottomLeftData[0] == 90);
	CHECK(bottomLeftData[1] == 80);
	CHECK(bottomLeftData[2] == 70);
	CHECK(bottomLeftData[3] == 255);
	CHECK(bottomLeftData[4] == 120);
	CHECK(bottomLeftData[5] == 110);
	CHECK(bottomLeftData[6] == 100);
	CHECK(bottomLeftData[7] == 255);
	CHECK(bottomLeftData[8] == 30);
	CHECK(bottomLeftData[9] == 20);
	CHECK(bottomLeftData[10] == 10);
	CHECK(bottomLeftData[11] == 255);
	CHECK(bottomLeftData[12] == 60);
	CHECK(bottomLeftData[13] == 50);
	CHECK(bottomLeftData[14] == 40);
	CHECK(bottomLeftData[15] == 255);

	// Bit 5 is set, so data row 0 is already the top row and stays in place.
	std::vector<unsigned char> topLeftBytes = topLeftHeader;
	topLeftBytes.insert(topLeftBytes.end(), pixels.begin(), pixels.end());
	const std::filesystem::path topLeftPath = tempImagePath();
	writeFile(topLeftPath, topLeftBytes);

	Image topLeft;
	REQUIRE_NOTHROW(topLeft.load(topLeftPath.string()));
	std::filesystem::remove(topLeftPath);
	REQUIRE(topLeft.getHeight() == 2);
	const unsigned char* topLeftData = topLeft.getData();
	CHECK(topLeftData[0] == 30);
	CHECK(topLeftData[1] == 20);
	CHECK(topLeftData[2] == 10);
	CHECK(topLeftData[3] == 255);
	CHECK(topLeftData[4] == 60);
	CHECK(topLeftData[5] == 50);
	CHECK(topLeftData[6] == 40);
	CHECK(topLeftData[7] == 255);
	CHECK(topLeftData[8] == 90);
	CHECK(topLeftData[9] == 80);
	CHECK(topLeftData[10] == 70);
	CHECK(topLeftData[11] == 255);
	CHECK(topLeftData[12] == 120);
	CHECK(topLeftData[13] == 110);
	CHECK(topLeftData[14] == 100);
	CHECK(topLeftData[15] == 255);
}

TEST_CASE("Image rejects a TGA with a truncated pixel block", "[image]")
{
	const std::vector<unsigned char> header = {
		0, 0, 2,
		0, 0, 0, 0, 0,
		0, 0,
		0, 0,
		64, 0,
		64, 0,
		24,
		0
	};
	// A 64x64 24-bit image needs 12288 bytes of pixel data but only 3 are present.
	const std::vector<unsigned char> pixels = { 10, 20, 30 };

	std::vector<unsigned char> bytes = header;
	bytes.insert(bytes.end(), pixels.begin(), pixels.end());

	const std::filesystem::path path = tempImagePath();
	writeFile(path, bytes);

	Image image;
	CHECK_THROWS(image.load(path.string()));
	std::filesystem::remove(path);
}

TEST_CASE("Image rejects a TGA whose declared size exceeds the file", "[image]")
{
	// A 2048x2048 32-bit image needs 16 MiB of pixel data while the file holds
	// 3 bytes, so the header is lying about the size of the pixel block.
	const std::vector<unsigned char> header = {
		0, 0, 2,
		0, 0, 0, 0, 0,
		0, 0,
		0, 0,
		0, 8,
		0, 8,
		32,
		0
	};
	const std::vector<unsigned char> pixels = { 10, 20, 30 };

	std::vector<unsigned char> bytes = header;
	bytes.insert(bytes.end(), pixels.begin(), pixels.end());

	const std::filesystem::path path = tempImagePath();
	writeFile(path, bytes);

	Image image;
	bool threw = false;
	std::string message;
	try
	{
		image.load(path.string());
	}
	catch (const std::runtime_error& error)
	{
		threw = true;
		message = error.what();
	}
	std::filesystem::remove(path);
	REQUIRE(threw);
	// The declared size is compared against the bytes left in the file, so the
	// pixel buffer is never allocated for data that is not there.
	const bool reportsDeclaredSize = message.find("exceeds") != std::string::npos;
	CHECK(reportsDeclaredSize);
}

TEST_CASE("Image rejects a TGA bit depth that is not 24 or 32", "[image]")
{
	const std::vector<unsigned char> header = {
		0, 0, 2,
		0, 0, 0, 0, 0,
		0, 0,
		0, 0,
		2, 0,
		1, 0,
		26,
		0
	};
	// 26 bits per pixel is not a valid TGA depth. Dividing by 8 turns it into
	// 3 bytes per pixel, which would read the image with the wrong stride and
	// silently produce garbage pixels.
	const std::vector<unsigned char> pixels = { 10, 20, 30, 40, 50, 60 };

	std::vector<unsigned char> bytes = header;
	bytes.insert(bytes.end(), pixels.begin(), pixels.end());

	const std::filesystem::path path = tempImagePath();
	writeFile(path, bytes);

	Image image;
	CHECK_THROWS(image.load(path.string()));
	std::filesystem::remove(path);
}

TEST_CASE("Image rejects a TGA with missing pixel data", "[image]")
{
	const std::vector<unsigned char> header = {
		0, 0, 2,
		0, 0, 0, 0, 0,
		0, 0,
		0, 0,
		2, 0,
		1, 0,
		24,
		0
	};
	// A 2x1 24-bit image needs 6 bytes of pixel data but only 3 are present.
	const std::vector<unsigned char> pixels = { 10, 20, 30 };

	std::vector<unsigned char> bytes = header;
	bytes.insert(bytes.end(), pixels.begin(), pixels.end());

	const std::filesystem::path path = tempImagePath();
	writeFile(path, bytes);

	Image image;
	CHECK_THROWS(image.load(path.string()));
	std::filesystem::remove(path);
}

TEST_CASE("Image rejects a TGA with invalid dimensions", "[image]")
{
	const std::vector<unsigned char> header = {
		0, 0, 2,
		0, 0, 0, 0, 0,
		0, 0,
		0, 0,
		0, 0x80, // Width 0x8000 is negative when read as a signed short.
		1, 0,
		24,
		0
	};

	const std::filesystem::path path = tempImagePath();
	writeFile(path, header);

	Image image;
	CHECK_THROWS(image.load(path.string()));
	std::filesystem::remove(path);
}

TEST_CASE("Image rejects an 8-bit grayscale TGA", "[image]")
{
	const std::vector<unsigned char> header = {
		0, 0, 3,
		0, 0, 0, 0, 0,
		0, 0,
		0, 0,
		2, 0,
		1, 0,
		8,
		0
	};
	const std::vector<unsigned char> pixels = { 100, 200 };

	std::vector<unsigned char> bytes = header;
	bytes.insert(bytes.end(), pixels.begin(), pixels.end());

	const std::filesystem::path path = tempImagePath();
	writeFile(path, bytes);

	Image image;
	CHECK_THROWS(image.load(path.string()));
	std::filesystem::remove(path);
}
