# Starter CMake skeleton, "Mac" track (Metal).
# Build:
#   cmake -S . -B build && cmake --build build   # Makefiles / Ninja
#   ./build/cube_metal
#   cmake -S . -B build-xcode -G Xcode           # optional: Xcode project (GPU Capture)
cmake_minimum_required(VERSION 3.20)
project(cube_metal LANGUAGES C CXX OBJCXX)

if(NOT APPLE)
  message(FATAL_ERROR "The Mac track builds on macOS only. On other systems use starter/vulkan.")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OBJCXX_STANDARD 17)
set(CMAKE_OSX_DEPLOYMENT_TARGET "14.0" CACHE STRING "Minimum macOS version")

set(BHS_ASSET_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../assets")
get_filename_component(BHS_ASSET_DIR "${BHS_ASSET_DIR}" ABSOLUTE)

add_executable(cube_metal
  src/main.mm
  src/shaders.metal          # listed for IDE visibility only; compiled at runtime
  ../common/obj_loader.h
  ../common/math3d.h
)
set_source_files_properties(src/shaders.metal PROPERTIES HEADER_FILE_ONLY ON)

target_compile_options(cube_metal PRIVATE -fobjc-arc -Wall -Wextra)
target_compile_definitions(cube_metal PRIVATE
  BHS_ASSET_DIR="${BHS_ASSET_DIR}"
  BHS_SHADER_PATH="${CMAKE_CURRENT_SOURCE_DIR}/src/shaders.metal"
)
target_link_libraries(cube_metal PRIVATE
  "-framework Metal"
  "-framework MetalKit"
  "-framework AppKit"
  "-framework QuartzCore"
)

# Optional: with full Xcode (`xcrun -f metal`) you can build a .metallib at
# build time via add_custom_command and load it instead of the source.
