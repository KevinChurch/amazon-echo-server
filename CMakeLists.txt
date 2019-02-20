cmake_minimum_required(VERSION 3.10.0)
project(amazon-echo-server)

if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "In-source builds not allowed.
    Please make a new directory (called a build directory) and run CMake from there.
    You may need to remove CMakeCache.txt." )
endif()

if (CMAKE_BUILD_TYPE STREQUAL "" AND "$ENV{DEVEL_SHELL}" STREQUAL "1")
    message(STATUS "Setting debug build type by default in devel shell")
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(CTest)
enable_testing()

include(GoogleTest)
add_subdirectory(/usr/src/googletest googletest)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.50 REQUIRED COMPONENTS system log)
message(STATUS "Boost version: ${Boost_VERSION}")

include_directories(include)

add_library(config_parser_lib src/config_parser.cc src/config_parser_exception.cc)
add_library(server_lib src/server.cc)
add_library(reply_lib src/reply.cc)
add_library(request_lib src/request.cc)
add_library(session_lib src/session.cc)
add_library(handler_lib src/echo_handler.cc src/static_handler.cc src/not_found_handler.cc src/status_handler.cc src/request.cc src/reply.cc src/handler_manager.cc)
add_library(logging_lib src/logging.cc)

add_executable(server src/server_main.cc src/session.cc)
target_link_libraries(server
    PRIVATE
        config_parser_lib
        server_lib
	request_lib
        reply_lib
      	handler_lib
        logging_lib
      	Boost::system
      	Boost::log_setup
        Boost::log
)

add_executable(config_parser_test tests/config_parser_test.cc)
target_link_libraries(config_parser_test config_parser_lib gtest_main)

add_executable(reply_test tests/reply_test.cc)
target_link_libraries(reply_test reply_lib gtest_main)

add_executable(request_test tests/request_test.cc)
target_link_libraries(request_test request_lib logging_lib gtest_main Boost::system Boost::log)

add_executable(session_test tests/session_test.cc)
target_link_libraries(session_test session_lib request_lib reply_lib gtest_main Boost::system)

add_executable(echo_handler_test tests/echo_handler_test.cc)
target_link_libraries(echo_handler_test handler_lib logging_lib gtest_main Boost::system Boost::log)

add_executable(not_found_handler_test tests/not_found_handler_test.cc)
target_link_libraries(not_found_handler_test request_lib logging_lib reply_lib config_parser_lib handler_lib gtest_main Boost::system Boost::log)

add_executable(status_handler_test tests/status_handler_test.cc)
target_link_libraries(status_handler_test request_lib logging_lib reply_lib config_parser_lib handler_lib gtest_main Boost::system Boost::log)

gtest_discover_tests(config_parser_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(reply_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(request_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(session_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(echo_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(not_found_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(status_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)

include(cmake/CodeCoverageReportConfig.cmake)
generate_coverage_report(TARGETS server config_parser_lib request_lib reply_lib session_lib handler_lib logging_lib TESTS config_parser_test request_test reply_test session_test echo_handler_test not_found_handler_test status_handler_test)


add_test(NAME INTEGRATION_TEST COMMAND integration_test.sh WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
