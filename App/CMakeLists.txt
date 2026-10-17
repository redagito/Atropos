# WIN32 switches the Windows subsystem to GUI so no console window appears.
add_executable(app WIN32
    source/Main.cpp
)

target_link_libraries(app PRIVATE
    atropos
    spdlog::spdlog
)

# Keep main() as the entry point instead of requiring WinMain.
if(MSVC)
    target_link_options(app PRIVATE /ENTRY:mainCRTStartup)
endif()

# The game loads startup.cfg and assets relative to the working directory, so
# default the debugger working directory to the app folder.
set_target_properties(app PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

# Also place startup.cfg and the data assets next to the built executable so it
# can be run directly from the build directory.
add_custom_command(TARGET app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/startup.cfg"
        "$<TARGET_FILE_DIR:app>/startup.cfg"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/data"
        "$<TARGET_FILE_DIR:app>/data"
    COMMENT "Copying startup.cfg and data next to app"
    VERBATIM
)
