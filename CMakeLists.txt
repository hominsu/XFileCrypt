cmake_minimum_required(VERSION 3.19)
project(XFileCrypt)

set(CMAKE_CXX_STANDARD 17)

# ============================== select operating system  ==============================
message("")
message("Operation system is ${CMAKE_SYSTEM}")
if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    message("Current platform: Linux")
    link_directories(/usr/local/lib)
    include_directories(/usr/local/include)
    message(STATUS "Load link directories from /usr/local/lib")
    message(STATUS "Load include directories from /usr/local/include")
elseif (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    message("Current platform: Darwin")
    link_directories(/opt/homebrew/lib)
    include_directories(/opt/homebrew/include)
    message(STATUS "Load link directories from /opt/homebrew/lib")
    message(STATUS "Load include directories from /opt/homebrew/include")
    SET(OPENSSL_ROOT_DIR /opt/homebrew/Cellar/openssl@1.1/1.1.1l_1)
    SET(OPENSSL_INCLUDE_DIR /opt/homebrew/Cellar/openssl@1.1/1.1.1l_1/include)
elseif (CMAKE_SYSTEM_NAME MATCHES "Windows")
    message("Current platform: Windows")
else ()
    message(FATAL_ERROR "Platform ${CMAKE_SYSTEM_NAME} is not support for this project")
endif ()


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
        "crypt/*.cc"
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

add_executable(${PROJECT_NAME} ${ProjectSRC})
target_link_libraries(${PROJECT_NAME} OpenSSL::Crypto Threads::Threads -static)
