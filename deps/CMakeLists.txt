#ImGui
File(GLOB   SOURCE_FILES imgui/*.cpp )
File(GLOB   HEADER_FILES imgui/*.h   )
File(GLOB   BACKEND_SOUR imgui/backends/imgui_impl_glfw.cpp )
File(GLOB   BACKEND_HEAD imgui/backends/imgui_impl_glfw.h   )
File(GLOB   BACKEND_SOUR2 imgui/backends/imgui_impl_opengl3.cpp )
File(GLOB   BACKEND_HEAD2 imgui/backends/imgui_impl_opengl3.h   )

set(IMGUI_FILES ${SOURCE_FILES} ${HEADER_FILES} ${BACKEND_SOUR} ${BACKEND_HEAD} ${BACKEND_SOUR2} ${BACKEND_HEAD2} PARENT_SCOPE)
