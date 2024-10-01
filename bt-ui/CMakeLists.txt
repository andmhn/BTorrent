cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

set(EXECUTABLE ${CMAKE_PROJECT_NAME})
project(${EXECUTABLE} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SRCS 
"main.cpp"
"bt-ui.cpp"
"opensans_font.cpp")

add_executable(${EXECUTABLE} ${SRCS})
target_include_directories(${EXECUTABLE} PRIVATE "../bt-core")

# GLFW
#====================================================
set(GLFW_DIR ${PROJECT_SOURCE_DIR}/external/glfw-3.4)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(${GLFW_DIR})
target_include_directories(${EXECUTABLE} PRIVATE "${GLFW_DIR}/include")

# ImGUI
#====================================================
set(IMGUI_DIR ${PROJECT_SOURCE_DIR}/external/imgui-1.91.1)

add_library("imgui" "${IMGUI_DIR}/imgui.cpp"
#                    "${IMGUI_DIR}/imgui_demo.cpp"
                    "${IMGUI_DIR}/imgui_draw.cpp"
                    "${IMGUI_DIR}/imgui_tables.cpp"
                    "${IMGUI_DIR}/imgui_widgets.cpp"          
                    "${IMGUI_DIR}/backends/imgui_impl_glfw.cpp"
                    "${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp")				  
target_include_directories("imgui" PRIVATE "${IMGUI_DIR}" "${GLFW_DIR}/include")
target_include_directories(${EXECUTABLE} PRIVATE "${IMGUI_DIR}")
target_include_directories(${EXECUTABLE} PRIVATE "${IMGUI_DIR}/backends")

# NFD   nativefiledialog-extended
#=====================================================
set(NFD_DIR ${PROJECT_SOURCE_DIR}/external/nativefiledialog-extended-1.2.1)
add_subdirectory(${NFD_DIR})
target_include_directories(${EXECUTABLE} PRIVATE "${NFD_DIR}/src/include")


# Platform specific
#=====================================================
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  # enables hot reloading for debug
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /ZI")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SAFESEH:NO")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
  add_compile_definitions("GLFW_EXPOSE_NATIVE_WIN32")

  # disables terminal in release mode
  if(NO_CONSOLE)
    add_compile_definitions("NO_CONSOLE")
  endif()
  target_link_libraries(${EXECUTABLE} glfw imgui opengl32 gdi32 shell32 nfd bt-core)

else() # assume using linux
  add_compile_definitions("GLFW_EXPOSE_NATIVE_X11")
  find_package(OpenGL REQUIRED)
  target_link_libraries(${EXECUTABLE} glfw imgui OpenGL nfd bt-core)
endif()