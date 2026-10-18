add_executable(atropos_tests
    source/ConfigFileTest.cpp
    source/AudioDecoderTest.cpp
    source/ObjModelLoaderTest.cpp
    source/ImageTest.cpp
    source/FileLoaderTest.cpp
    source/AABBoxTest.cpp
    source/GameObjectTest.cpp
    source/ControllerTest.cpp
    source/CollisionSystemTest.cpp
    source/HealthControllerTest.cpp
    source/GameSystemTest.cpp
    source/RenderableTest.cpp
    source/MeshTest.cpp
    source/ThreadPoolTest.cpp
    source/ResourceHandleTest.cpp
    source/AsyncLoaderTest.cpp
    source/SceneTest.cpp
    source/MenuShipTest.cpp
    source/CameraTest.cpp
    source/GameStateTest.cpp
    source/ShaderSmokeTest.cpp
    source/UITest.cpp
    source/ScoreAndHealthTest.cpp
)

target_compile_definitions(atropos_tests PRIVATE
    ATROPOS_SOUND_DIR="${CMAKE_SOURCE_DIR}/App/data/sound"
    ATROPOS_SHADER_SRC_DIR="${CMAKE_SOURCE_DIR}/App/data/shader/src"
    ATROPOS_TEXTURE_DIR="${CMAKE_SOURCE_DIR}/App/data/texture"
    ATROPOS_FONT_DIR="${CMAKE_SOURCE_DIR}/App/data/font"
    ATROPOS_DATA_DIR="${CMAKE_SOURCE_DIR}/App/data"
)

target_link_libraries(atropos_tests PRIVATE
    atropos
    Catch2::Catch2WithMain
)

include(Catch)
catch_discover_tests(atropos_tests)
