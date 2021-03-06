CMAKE_MINIMUM_REQUIRED(VERSION 3.19)

project(crypt_test)

file(GLOB ProjectSRC
        "../../crypt/*.cc"
        "*.cc")

add_executable(${PROJECT_NAME} ${ProjectSRC})
target_link_libraries(${PROJECT_NAME} OpenSSL::Crypto)

macro(do_test ARG1 ARG2)
    ADD_TEST(NAME ${PROJECT_NAME}_${ARG1}_${ARG2}
            COMMAND ${PROJECT_NAME} ${ARG1} ${ARG2})
    SET_TESTS_PROPERTIES(${PROJECT_NAME}_${ARG1}_${ARG2}
            PROPERTIES PASS_REGULAR_EXPRESSION ${ARG2})
endmacro(do_test)

do_test("1122334455667788" "apple!!!")
do_test("8877665544332211" "hello!!!")
do_test("1234567887654321" "hello the world")
do_test("8765432112345678" "hello the world")
do_test("apple!!!apple!!!" "000 111 222 333 444")
do_test("hello!!!hello!!!" "555 666 777 888 999")
