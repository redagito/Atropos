add_executable(atropos_tests
    source/ConfigFileTest.cpp
    source/AudioDecoderTest.cpp
    source/ObjModelLoaderTest.cpp
    source/ImageTest.cpp
    source/FileLoaderTest.cpp
    source/AABBoxTest.cpp
    source/GameObjectTest.cpp
    source/CollisionSystemTest.cpp
    source/HealthControllerTest.cpp
    source/GameSystemTest.cpp
)

target_compile_definitions(atropos_tests PRIVATE
    ATROPOS_SOUND_DIR="${CMAKE_SOURCE_DIR}/App/data/sound"
)

target_link_libraries(atropos_tests PRIVATE
    atropos
    Catch2::Catch2WithMain
)

include(Catch)
catch_discover_tests(atropos_tests)
