#include "io/FileLoader.h"

#include <catch2/catch_test_macros.hpp>

#include <process.h>

#include <filesystem>
#include <fstream>
#include <string>

namespace
{
	std::filesystem::path tempTextPath()
	{
		// ctest gives every test case its own process, so the counter restarts
		// at zero everywhere and would collide across concurrent runs. The
		// process id makes the name unique among simultaneously running tests.
		static int counter = 0;
		return std::filesystem::temp_directory_path()
			/ ("atropos_file_" + std::to_string(_getpid()) + "_" + std::to_string(counter++) + ".txt");
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
	REQUIRE_NOTHROW(loader.load());
	const std::string first = loader.getContent();

	REQUIRE_NOTHROW(loader.load());
	const std::string second = loader.getContent();

	std::filesystem::remove(path);

	REQUIRE_FALSE(first.empty());
	CHECK(first == "line one\nline two\nline three\n");
	CHECK(first == second);
}

TEST_CASE("FileLoader throws when the file cannot be opened", "[fileloader]")
{
	FileLoader loader("definitely_missing_atropos_file.txt");

	CHECK_THROWS(loader.load());
}
