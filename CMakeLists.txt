cmake_minimum_required(VERSION 3.20)

project(Atropos LANGUAGES C CXX)

# The code base predates C++17 (it still uses dynamic exception specifications),
# so the initial build stays on C++14. See the README TODO on modernizing.
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Catch2 CONFIG REQUIRED)
find_package(OpenAL REQUIRED)
find_package(minimp3 CONFIG REQUIRED)

enable_testing()

add_subdirectory(Atropos)
add_subdirectory(ConsoleLauncher)
add_subdirectory(Test)
