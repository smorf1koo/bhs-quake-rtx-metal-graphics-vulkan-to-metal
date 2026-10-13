# Starter CMake skeleton, "no Mac" track (Vulkan 1.3 + GLFW).
# Requires Vulkan SDK 1.3+ (https://vulkan.lunarg.com/) and GLFW 3.3+.
# Build:
#   cmake -S . -B build && cmake --build build
#   ./build/cube_vulkan        # Windows: build\Debug\cube_vulkan.exe
# Without a system GLFW: -DBHS_FETCH_GLFW=ON (downloads GLFW into build/).
cmake_minimum_required(VERSION 3.20)
project(cube_vulkan LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BHS_FETCH_GLFW "Download GLFW with FetchContent instead of find_package" OFF)

find_package(Vulkan 1.3 REQUIRED)

if(BHS_FETCH_GLFW)
  include(FetchContent)
  set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
  set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
  set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
  FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw.git GIT_TAG 3.4)
  FetchContent_MakeAvailable(glfw)
else()
  find_package(glfw3 3.3 REQUIRED)
endif()

set(BHS_ASSET_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../assets")
get_filename_component(BHS_ASSET_DIR "${BHS_ASSET_DIR}" ABSOLUTE)

# --- Shaders: GLSL -> SPIR-V with glslc from the Vulkan SDK ------------------
find_program(GLSLC glslc HINTS "$ENV{VULKAN_SDK}/bin" "$ENV{VULKAN_SDK}/Bin")
if(NOT GLSLC)
  message(FATAL_ERROR "glslc not found. Install the Vulkan SDK and set VULKAN_SDK.")
endif()

set(BHS_SHADER_OUT "${CMAKE_CURRENT_BINARY_DIR}/shaders")
file(MAKE_DIRECTORY "${BHS_SHADER_OUT}")
set(BHS_SPIRV "")
foreach(SHADER cube.vert cube.frag)
  set(SRC "${CMAKE_CURRENT_SOURCE_DIR}/shaders/${SHADER}")
  set(DST "${BHS_SHADER_OUT}/${SHADER}.spv")
  add_custom_command(OUTPUT "${DST}"
    COMMAND "${GLSLC}" --target-env=vulkan1.3 -o "${DST}" "${SRC}"
    DEPENDS "${SRC}" COMMENT "glslc ${SHADER}" VERBATIM)
  list(APPEND BHS_SPIRV "${DST}")
endforeach()
add_custom_target(cube_shaders DEPENDS ${BHS_SPIRV})

add_executable(cube_vulkan src/main.cpp src/vk_base.cpp src/vk_base.h
  src/image_io.cpp src/image_io.h ../common/obj_loader.h ../common/math3d.h)
add_dependencies(cube_vulkan cube_shaders)
target_link_libraries(cube_vulkan PRIVATE Vulkan::Vulkan glfw)
# stb_image.h v2.30 (public domain / MIT, https://github.com/nothings/stb,
# commit 013ac3beddff3dbffafd5177e7972067cd2b5083), used by src/image_io.cpp.
target_include_directories(cube_vulkan SYSTEM PRIVATE third_party/stb)
target_compile_definitions(cube_vulkan PRIVATE
  BHS_ASSET_DIR="${BHS_ASSET_DIR}"
  BHS_SHADER_DIR="${BHS_SHADER_OUT}"
  GLFW_INCLUDE_VULKAN
)
if(MSVC)
  target_compile_options(cube_vulkan PRIVATE /W4)
else()
  target_compile_options(cube_vulkan PRIVATE -Wall -Wextra)
endif()
