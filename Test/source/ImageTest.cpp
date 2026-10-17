#include "resource/Image.h"

#include <catch2/catch_test_macros.hpp>

#include <filesystem>
#include <fstream>
#include <random>
#include <string>
#include <vector>

namespace
{
	std::filesystem::path tempImagePath()
	{
		static std::mt19937 generator(std::random_device{}());
		return std::filesystem::temp_directory_path()
			/ ("atropos_img_" + std::to_string(generator()) + ".tga");
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
	const bool result = image.load(path.string());
	std::filesystem::remove(path);

	REQUIRE(result);
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
	const bool result = image.load(path.string());
	std::filesystem::remove(path);

	REQUIRE(result);
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
	const bool result = image.load(path.string());
	std::filesystem::remove(path);

	CHECK_FALSE(result);
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
	const bool result = image.load(path.string());
	std::filesystem::remove(path);

	CHECK_FALSE(result);
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
	const bool result = image.load(path.string());
	std::filesystem::remove(path);

	CHECK_FALSE(result);
}
