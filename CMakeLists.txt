project(embeddedRTPS-Desktop)
cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-O0 -Wall -Wextra -Wnon-virtual-dtor -Wpedantic -fno-non-call-exceptions -fno-exceptions -fPIC -fno-rtti")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Add micro-CDR
add_subdirectory(embeddedrtps/thirdparty/Micro-CDR)

# Add lwIP
SET(LWIP_CONTRIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/contrib")
SET(LWIP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lwip")

SET(PORT_ROOT "${LWIP_CONTRIB_DIR}/ports/unix")
SET(ARCH_DIR "${PORT_ROOT}/port/include")

include(${LWIP_CONTRIB_DIR}/ports/CMakeCommon.cmake)

set (LWIP_DEFINITIONS -DLWIP_DEBUG)
set (LWIP_INCLUDE_DIRS
        "${LWIP_DIR}/src/include"
        "${LWIP_CONTRIB_DIR}"
        "${ARCH_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/lwip"
        "${WPDPACK_INCLUDE_DIR}"
        )

include(${LWIP_DIR}/src/Filelists.cmake)
include(${LWIP_CONTRIB_DIR}/Filelists.cmake)
include(${PORT_ROOT}/Filelists.cmake)

add_library(lwip STATIC ${lwipcore_SRCS} ${lwipcore3_SRCS} ${lwipcontribportunix_SRCS})

target_include_directories(lwip PUBLIC ${LWIP_INCLUDE_DIRS})
include_directories(lwip PUBLIC embeddedrtps/thirdparty/lwip/)

target_compile_options(lwip PRIVATE ${LWIP_COMPILER_FLAGS})
target_compile_definitions(lwip PRIVATE ${LWIP_DEFINITIONS})


target_link_libraries(lwip ${LWIP_SANITIZER_LIBS}  lwipcontribaddons lwipallapps lwipcore lwipcontribportunix)

# embeddedRTPS
add_library(embeddedrtps "")
include_directories(include)
include_directories(include/lwip)
include_directories(embeddedrtps/include)
include_directories(${microcdr_SOURCE_DIR}/include)
target_include_directories(embeddedrtps PUBLIC embeddedrtps/include include ${LWIP_DIR}/src/include ${microcdr_SOURCE_DIR}/include)

target_sources(embeddedrtps
        PRIVATE
        "embeddedrtps/src/rtps.cpp"
        "embeddedrtps/src/ThreadPool.cpp"
        "embeddedrtps/src/communication/UdpDriver.cpp"
        "embeddedrtps/src/discovery/ParticipantProxyData.cpp"
        "embeddedrtps/src/discovery/SPDPAgent.cpp"
        "embeddedrtps/src/discovery/SEDPAgent.cpp"
        "embeddedrtps/src/discovery/TopicData.cpp"
        "embeddedrtps/src/entities/Domain.cpp"
        "embeddedrtps/src/entities/Participant.cpp"
        "embeddedrtps/src/entities/StatelessReader.cpp"
        "embeddedrtps/src/messages/MessageReceiver.cpp"
        "embeddedrtps/src/messages/MessageTypes.cpp"
        "embeddedrtps/src/storages/HistoryCache.cpp"
        "embeddedrtps/src/storages/SimpleHistoryCache.cpp"
        "embeddedrtps/src/storages/PBufWrapper.cpp"
)

target_include_directories(embeddedrtps PUBLIC include ${microcdr_SOURCE_DIR}/include)
target_link_libraries(embeddedrtps PRIVATE lwip microcdr)

# embeddedRTPS test
add_subdirectory(tests)

# FastDDS
add_subdirectory(fastdds)
