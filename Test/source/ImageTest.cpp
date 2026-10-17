#include "resource/Image.h"

#include <catch2/catch_test_macros.hpp>

#include <filesystem>
#include <fstream>
#include <random>
#include <string>
#include <vector>

namespace
{
	std::filesystem::path tempImagePath(const std::string& extension = ".tga")
	{
		static std::mt19937 generator(std::random_device{}());
		return std::filesystem::temp_directory_path()
			/ ("atropos_img_" + std::to_string(generator()) + extension);
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

TEST_CASE("Image loads an RGBA PNG", "[image]")
{
	const std::vector<unsigned char> bytes = {
		0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 0x00, 0x0D,
		0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x01,
		0x08, 0x06, 0x00, 0x00, 0x00, 0xF4, 0x22, 0x7F, 0x8A, 0x00, 0x00, 0x00,
		0x01, 0x73, 0x52, 0x47, 0x42, 0x00, 0xAE, 0xCE, 0x1C, 0xE9, 0x00, 0x00,
		0x00, 0x04, 0x67, 0x41, 0x4D, 0x41, 0x00, 0x00, 0xB1, 0x8F, 0x0B, 0xFC,
		0x61, 0x05, 0x00, 0x00, 0x00, 0x09, 0x70, 0x48, 0x59, 0x73, 0x00, 0x00,
		0x0E, 0xC3, 0x00, 0x00, 0x0E, 0xC3, 0x01, 0xC7, 0x6F, 0xA8, 0x64, 0x00,
		0x00, 0x00, 0x11, 0x49, 0x44, 0x41, 0x54, 0x18, 0x57, 0x63, 0xE0, 0x12,
		0x91, 0xFB, 0xAF, 0x61, 0x64, 0xF3, 0x1F, 0x00, 0x09, 0x41, 0x02, 0xD1,
		0x4F, 0x35, 0x98, 0xF7, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4E, 0x44,
		0xAE, 0x42, 0x60, 0x82
	};

	const std::filesystem::path path = tempImagePath(".png");
	writeFile(path, bytes);

	Image image;
	const bool result = image.load(path.string());
	std::filesystem::remove(path);

	REQUIRE(result);
	REQUIRE(image.getWidth() == 2);
	REQUIRE(image.getHeight() == 1);
	const unsigned char* data = image.getData();
	CHECK(data[0] == 10);
	CHECK(data[1] == 20);
	CHECK(data[2] == 30);
	CHECK(data[3] == 255);
	CHECK(data[4] == 40);
	CHECK(data[5] == 50);
	CHECK(data[6] == 60);
	CHECK(data[7] == 255);
}
