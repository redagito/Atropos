cmake_minimum_required(VERSION 3.20)

project(Atropos LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Catch2 CONFIG REQUIRED)
find_package(OpenAL REQUIRED)
find_package(minimp3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)

enable_testing()

# Treat warnings as errors for every target in every build, and raise the
# warning level as high as the compiler goes.
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Werror)
endif()

add_subdirectory(Atropos)
add_subdirectory(App)
add_subdirectory(Test)
