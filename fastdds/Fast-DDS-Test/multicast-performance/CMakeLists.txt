# Copyright 2016 Proyectos y Sistemas de Mantenimiento SL (eProsima).
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cmake_minimum_required(VERSION 2.8.12)

if(NOT CMAKE_VERSION VERSION_LESS 3.0)
    cmake_policy(SET CMP0048 NEW)
endif()

project(MulticastPerformance_fastdds)

message(STATUS "Configuring Multicast Performance Test...")
file(GLOB MULTICASTPERFORMANCE_SOURCES_CXX "*.cxx")
file(GLOB MULTICASTPERFORMANCE_SOURCES_CPP "*.cpp")

add_executable(MulticastPerformance_fastdds ${MULTICASTPERFORMANCE_SOURCES_CXX} ${MULTICASTPERFORMANCE_SOURCES_CPP})
target_link_libraries(MulticastPerformance_fastdds fastrtps fastcdr foonathan_memory)
install(TARGETS MulticastPerformance_fastdds
    RUNTIME DESTINATION MulticastPerformance/${BIN_INSTALL_DIR})
