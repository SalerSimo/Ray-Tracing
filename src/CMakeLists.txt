file(GLOB SOURCES "*.c")

add_executable(RayTracing ${SOURCES})

target_include_directories(RayTracing PRIVATE ${PROJECT_SOURCE_DIR}/external/include)

# Link to SDL3 static or import library (.lib, .a), NOT the DLL
target_link_libraries(RayTracing PRIVATE
    ${PROJECT_SOURCE_DIR}/external/lib/SDL3.dll  # or .lib or .dll.a depending on your SDL3 build
)

# Copy the DLL to output directory after build
add_custom_command(TARGET RayTracing POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${PROJECT_SOURCE_DIR}/external/lib/SDL3.dll"
    $<TARGET_FILE_DIR:RayTracing>
)
