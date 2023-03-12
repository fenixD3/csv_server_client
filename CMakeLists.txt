cmake_minimum_required(VERSION 3.20)
project(opendev_test)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_VERBOSE_MAKEFILE ON)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")

option(SANITIZER_ANALYSIS "Sanitizers analysis" OFF)

if (SANITIZER_ANALYSIS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
        -O0 \
        -g \
        -fsanitize=thread \
    ")
endif()

#-fsanitize=address \
#-fsanitize=leak \
#-fsanitize=thread \
#-fsanitize=undefined \
#-fsanitize=memory

find_package(Boost 1.81.0 REQUIRED)
if (${Boost_FOUND})
    message(STATUS "Boost version: ${Boost_VERSION}")
    message(STATUS "Boost include dir: ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost library dir: ${Boost_LIBRARY_DIRS}")
    message(STATUS "Boost libs: ${Boost_LIBRARIES}")
else()
    message(FATAL_ERROR "Boost was not found")
endif()

find_package(Boost
    COMPONENTS
        serialization
     REQUIRED)

set(SOURCE_DIR csv_server_client)

file(GLOB_RECURSE SRCS ${SOURCE_DIR}/*.cpp)

add_executable(${PROJECT_NAME}
        ${SRCS}
        )

target_include_directories(${PROJECT_NAME} PRIVATE
        ${SOURCE_DIR}
        ${Boost_INCLUDE_DIRS}
        )

target_link_libraries(${PROJECT_NAME} PUBLIC
        Boost::serialization
        )

find_package(GTest REQUIRED)

if (${GTest_FOUND})
    set(CSV_READER_TESTS csv_reader_tests)
    add_executable(${CSV_READER_TESTS}
            ${SOURCE_DIR}/csv_use_case.cpp
            tests/csv_reader_tests.cpp)

    target_include_directories(${CSV_READER_TESTS} PRIVATE
            ${GTEST_INCLUDE_DIR}
            ${PROJECT_SOURCE_DIR})

    target_link_libraries(${CSV_READER_TESTS} PRIVATE
            ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES})

    set(MSG_SERIAL_TESTS msg_serial_tests)
    add_executable(${MSG_SERIAL_TESTS}
            ${SOURCE_DIR}/protocol_serializer.cpp
            tests/msg_serialization_tests.cpp)

    target_include_directories(${MSG_SERIAL_TESTS} PRIVATE
            ${GTEST_INCLUDE_DIR}
            ${PROJECT_SOURCE_DIR})

    target_link_libraries(${MSG_SERIAL_TESTS} PRIVATE
            ${GTEST_LIBRARIES}
            ${GTEST_MAIN_LIBRARIES}
            Boost::serialization)
endif()