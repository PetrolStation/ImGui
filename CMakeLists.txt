cmake_minimum_required(VERSION 3.16)

PA_NEW(ImGui)

PA_INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/imgui/backends
)

File(GLOB_RECURSE SOURCE_FILES src/*.cpp )
File(GLOB_RECURSE HEADER_FILES src/*.h   )

add_subdirectory(deps)

pa_add_source_files(
    ${IMGUI_FILES}
    ${SOURCE_FILES}
    ${HEADER_FILES}
)

