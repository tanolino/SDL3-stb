# General options
set(BUILD_SHARED_LIBS OFF CACHE BOOL "")
set(CMAKE_POSITION_INDEPENDENT_CODE ON CACHE BOOL "")

# SDL3
set(SDL_DISABLE_INSTALL ON CACHE BOOL "")
set(SDL_DISABLE_UNINSTALL ON CACHE BOOL "")
set(SDL_STATIC ON CACHE BOOL "")
set(SDL_STATIC_PIC ON CACHE BOOL "")
set(SDL_SHARED OFF CACHE BOOL "")
set(SDL_TESTS OFF CACHE BOOL "")
set(SDL_TEST_LIBRARY OFF CACHE BOOL "")
set(SDL_EXAMPLES OFF CACHE BOOL "")
add_subdirectory("SDL3")

# stb
set(STB_HEADER_DIR "${CMAKE_CURRENT_LIST_DIR}/stb" CACHE PATH "")

set(SKIP_INSTALL_LIBRARIES ON)
# zlib
set(ZLIB_BUILD_EXAMPLES OFF)
add_subdirectory("zlib")
set(SKIP_INSTALL_LIBRARIES OFF)