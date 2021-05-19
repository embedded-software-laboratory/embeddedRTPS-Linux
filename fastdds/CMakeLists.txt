cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-O2 -Wall -Wextra -fPIC")


if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/foonathan_memory_vendor/build/lib/foonathan_memory/cmake)
execute_process(COMMAND bash -c ${CMAKE_CURRENT_SOURCE_DIR}/compile_foonathan.sh  RESULT_VARIABLE compile_foonathan_ret OUTPUT_VARIABLE compile_foonathan_output)
message(${compile_foonathan_output})
if(NOT ${compile_foonathan_ret} MATCHES "0")
      message(FATAL_ERROR "Failed to compile foonathan memory: ${compile_foonathan_output}")
endif()
endif()
set(foonathan_memory_DIR "${CMAKE_CURRENT_SOURCE_DIR}/foonathan_memory_vendor/build/lib/foonathan_memory/cmake")

option(THIRDPARTY "" ON)
add_subdirectory(Fast-DDS)
add_subdirectory(Fast-DDS-Test)