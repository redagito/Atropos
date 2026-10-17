add_executable(consolelauncher
    source/Main.cpp
)

target_link_libraries(consolelauncher PRIVATE
    atropos
)

# The game loads startup.cfg and assets relative to the working directory, so
# default the debugger working directory to the launcher folder.
set_target_properties(consolelauncher PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

# Also place startup.cfg and the data assets next to the built executable so it
# can be run directly from the build directory.
add_custom_command(TARGET consolelauncher POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/startup.cfg"
        "$<TARGET_FILE_DIR:consolelauncher>/startup.cfg"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/data"
        "$<TARGET_FILE_DIR:consolelauncher>/data"
    COMMENT "Copying startup.cfg and data next to consolelauncher"
    VERBATIM
)
