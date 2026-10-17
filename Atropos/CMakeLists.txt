file(GLOB_RECURSE ATROPOS_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/source/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp
)

add_library(atropos STATIC
    ${ATROPOS_SOURCES}
)

target_include_directories(atropos
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/source
)

target_compile_definitions(atropos PRIVATE
    _CRT_SECURE_NO_WARNINGS
    GLFW_INCLUDE_NONE
)

target_link_libraries(atropos
    PUBLIC
        OpenGL::GL
        glfw
        glm::glm
        glad::glad
    PRIVATE
        OpenAL::OpenAL
        minimp3::minimp3
)
