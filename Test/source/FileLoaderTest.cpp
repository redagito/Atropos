#include "io/FileLoader.h"

#include <catch2/catch_test_macros.hpp>

#include <filesystem>
#include <fstream>
#include <random>
#include <string>

namespace
{
	std::filesystem::path tempTextPath()
	{
		static std::mt19937 generator(std::random_device{}());
		return std::filesystem::temp_directory_path()
			/ ("atropos_file_" + std::to_string(generator()) + ".txt");
	}
}

TEST_CASE("FileLoader reload does not duplicate content", "[fileloader]")
{
	const std::filesystem::path path = tempTextPath();
	{
		std::ofstream out(path);
		out << "line one\nline two\nline three\n";
	}

	FileLoader loader(path.string());
	REQUIRE(loader.load());
	const std::string first = loader.getContent();

	REQUIRE(loader.load());
	const std::string second = loader.getContent();

	std::filesystem::remove(path);

	REQUIRE_FALSE(first.empty());
	CHECK(first == "line one\nline two\nline three\n");
	CHECK(first == second);
}
