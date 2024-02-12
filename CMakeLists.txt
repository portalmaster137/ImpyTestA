cmake_minimum_required(VERSION 3.28)
project(ImpyTestA)

# use the FindMySQL.cmake module in cmake/
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

set(CMAKE_CXX_STANDARD 17)

find_package(MySQL REQUIRED)

if (MYSQL_INCLUDE_DIR AND MYSQL_LIBRARIES)
    message(STATUS "MySQL found")
else()
    message(FATAL_ERROR "MySQL not found")
endif()


add_subdirectory(external/spdlog)
add_subdirectory(external/DPP)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h")

## build executable

add_executable(ImpyTestA ${SOURCES} ${HEADERS})

target_include_directories(ImpyTestA PRIVATE include ${MYSQL_INCLUDE_DIR})

target_link_libraries(ImpyTestA PRIVATE spdlog::spdlog dpp ${MYSQL_LIBRARIES})  