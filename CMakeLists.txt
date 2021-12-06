cmake_minimum_required(VERSION 3.19)

# get git tag version
EXECUTE_PROCESS(COMMAND git describe --tags
        TIMEOUT 10
        OUTPUT_VARIABLE GIT_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )
string(REGEX MATCH "([0-9]+)\\.([0-9]+)\\.([0-9+])" BUILD_VERSION ${GIT_VERSION})

# setting project info
project(XFileCrypt
        LANGUAGES CXX
        VERSION ${BUILD_VERSION}
        HOMEPAGE_URL "https://github.com/hominsu/XFileCrypt")

# setting definitions
add_definitions(-DPROJECT_NAME=\"${PROJECT_NAME}\")
add_definitions(-DBUILD_VERSION=\"${BUILD_VERSION}\")
add_definitions(-DHOMEPAGE_URL=\"${PROJECT_HOMEPAGE_URL}\")
message("")
MESSAGE(STATUS "Building from git tag ${BUILD_VERSION}")
MESSAGE(STATUS "Home page url: ${PROJECT_HOMEPAGE_URL}")

# setting cpp standard version
set(CMAKE_CXX_STANDARD 17)


# ============================== select operating system  ==============================
message("")
message("Operation system is ${CMAKE_SYSTEM}")
message("Current compiler: ${CMAKE_CXX_COMPILER_ID}")
message("Current compiler version: ${CMAKE_CXX_COMPILER_VERSION}")
message("Current compiler directory: ${CMAKE_CXX_COMPILER}")
if (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    link_directories(/opt/homebrew/lib)
    include_directories(/opt/homebrew/include)
    message(STATUS "Load link directories from /opt/homebrew/lib")
    message(STATUS "Load include directories from /opt/homebrew/include")
    set(OPENSSL_ROOT_DIR /opt/homebrew/Cellar/openssl@1.1/1.1.1l_1)
    set(OPENSSL_INCLUDE_DIR /opt/homebrew/Cellar/openssl@1.1/1.1.1l_1/include)
    #    set(OPENSSL_ROOT_DIR /opt/homebrew/Cellar/openssl@3/3.0.0_1)
    #    set(OPENSSL_INCLUDE_DIR /opt/homebrew/Cellar/openssl@3/3.0.0_1/include)
elseif (CMAKE_SYSTEM_NAME MATCHES "Linux")
    link_directories(/usr/local/lib)
    include_directories(/usr/local/include)
    message(STATUS "Load link directories from /usr/local/lib")
    message(STATUS "Load include directories from /usr/local/include")
elseif (CMAKE_SYSTEM_NAME MATCHES "Windows")
    # setting vcpkg toolchain file
    set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake
            CACHE STRING "Vcpkg toolchain file")

    if (MSVC)   # if msvc
        # because opencv is use libcmt, so don't use dll
        # use the static C/CXX library for all build types; if Debug, set MultiThreadedDebug(MTd), else, set MultiThreaded(MT)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
        # setting charset
        add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
        add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
    elseif (MINGW)  # if mingw
        # setting vcpkg
        if (DEFINED ENV{VCPKG_DEFAULT_TRIPLET} AND NOT DEFINED VCPKG_TARGET_TRIPLET)
            set(VCPKG_TARGET_TRIPLET "$ENV{VCPKG_DEFAULT_TRIPLET}" CACHE STRING "")
        endif ()
    endif ()
else ()
    message(FATAL_ERROR "Platform ${CMAKE_SYSTEM} is not support for this project")
endif ()


# ============================== set output dir ==============================

# debug, ARCHIVE means static lib, LIBRARY means dynamic lib
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)

# release
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)


# ============================== find_package ==============================
# Threads
find_package(Threads REQUIRED)
if (Threads_FOUND)
    message("")
endif ()

# OpenSSL
set(OPENSSL_USE_STATIC_LIBS true)
find_package(OpenSSL REQUIRED)
if (OPENSSL_FOUND)
    message(STATUS "OPENSSL_VERSION = ${OPENSSL_VERSION}")
    message(STATUS "OPENSSL_INCLUDE_DIR = ${OPENSSL_INCLUDE_DIR}")
    message(STATUS "OPENSSL_LIBRARIES = ${OPENSSL_LIBRARIES}")
    message("")
endif ()


# ============================== add executable and link libraries ==============================
file(GLOB ProjectSRC
        "x_thread_pool/*.cc"
        "task/crypt/*.cc"
        "task/write/*.cc"
        "task/read/*.cc"
        "memory/*.cc"
        "task/*.cc"
        "*.cc")

if (CMAKE_BUILD_TYPE STREQUAL Debug)
    ADD_DEFINITIONS(-DDebug)
    message(STATUS "CMake Build Type: Debug")
    message("")
elseif (CMAKE_BUILD_TYPE STREQUAL Release)
    ADD_DEFINITIONS(-DRelease)
    message(STATUS "CMake Build Type: Release")
    message("")
endif ()

option(BUILD_TEST "Whether or not to build the test" ON)
if (${BUILD_TEST})
    message(STATUS "Building tests")
    enable_testing()
    add_subdirectory(ctest/crypt_test)
    add_subdirectory(ctest/file_test)
endif ()

# des dynamic lib and static lib
add_library(des_dynamic_lib SHARED "crypt/des_crypt.cc")
target_link_libraries(des_dynamic_lib OpenSSL::Crypto)
add_library(des_static_lib STATIC "crypt/des_crypt.cc")
target_link_libraries(des_static_lib OpenSSL::Crypto)

# aes dynamic lib and static lib
add_library(aes_dynamic_lib SHARED "crypt/aes_crypt.cc")
target_link_libraries(aes_dynamic_lib OpenSSL::Crypto)
add_library(aes_static_lib STATIC "crypt/aes_crypt.cc")
target_link_libraries(aes_static_lib OpenSSL::Crypto)


add_executable(${PROJECT_NAME}-${BUILD_VERSION}-${CMAKE_SYSTEM_NAME}-cli ${ProjectSRC})
target_link_libraries(${PROJECT_NAME}-${BUILD_VERSION}-${CMAKE_SYSTEM_NAME}-cli PUBLIC Threads::Threads aes_static_lib -static)
