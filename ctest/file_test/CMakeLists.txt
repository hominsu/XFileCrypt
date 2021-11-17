CMAKE_MINIMUM_REQUIRED(VERSION 3.20)

project(file_test)

# OpenSSL
set(OPENSSL_USE_STATIC_LIBS true)
find_package(OpenSSL REQUIRED)
if (OPENSSL_FOUND)
    message(STATUS "OPENSSL_VERSION = ${OPENSSL_VERSION}")
    message(STATUS "OPENSSL_INCLUDE_DIR = ${OPENSSL_INCLUDE_DIR}")
    message(STATUS "OPENSSL_LIBRARIES = ${OPENSSL_LIBRARIES}")
    message("")
endif ()

file(GLOB ProjectSRC
        "../../x_thread_pool/*.cc"
        "../../crypt/*.cc"
        "../../memory/*.cc"
        "../../task/*.cc"
        "*.cc")

add_executable(${PROJECT_NAME} ${ProjectSRC})
target_link_libraries(${PROJECT_NAME} OpenSSL::Crypto)

macro(do_test ARG1 ARG2)
    ADD_TEST(NAME ${PROJECT_NAME}_${ARG1}_${ARG2}
            COMMAND ${PROJECT_NAME} ${ARG1} ${ARG2})
    SET_TESTS_PROPERTIES(${PROJECT_NAME}_${ARG1}_${ARG2}
            PROPERTIES PASS_REGULAR_EXPRESSION ${ARG2})
endmacro(do_test)

do_test("1234567887654321" "hello the world")
do_test("8765432112345678" "hello the world")
do_test("apple!!!apple!!!" "000 111 222 333 444")
do_test("hello!!!apple!!!" "555 666 777 888 999")