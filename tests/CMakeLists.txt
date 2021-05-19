cmake_minimum_required(VERSION 3.10)

add_subdirectory(thirdparty/googletest)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
include_directories(${rtps_SOURCE_DIR}/include ${rtps_SOURCE_DIR})

add_executable(runTests
        RunAllTestsMain.cpp
        unittests/typesTest.cpp
        unittests/ThreadPoolTest.cpp
        unittests/communication/UdpConnectionTest.cpp
        unittests/discovery/ParticipantProxyDataTest.cpp
        unittests/discovery/BuiltInTopicDataTest.cpp
        unittests/discovery/SEDPTest.cpp
        unittests/storages/PBufWrapperTest.cpp
        unittests/entities/DomainTest.cpp
        unittests/entities/ParticipantTest.cpp
        unittests/entities/WriterTest.cpp
        unittests/entities/StatelessWriterTest.cpp
        unittests/entities/StatefulWriterTest.cpp
        unittests/entities/StatefulReaderTest.cpp
        unittests/entities/WriterProxyTest.cpp
        unittests/messages/MessageFactoryTest.cpp
        unittests/messages/MessageReceiverTest.cpp
        unittests/storages/MemoryPoolTest.cpp
        unittests/storages/HistoryCacheTest.cpp
        unittests/storages/SimpleHistoryCacheTest.cpp
        unittests/utils/udpUtilsTest.cpp
        )
target_link_libraries(runTests gtest gmock)
target_link_libraries(runTests embeddedrtps lwip)
target_include_directories(runTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/mocking")

add_executable(runThreadPoolDemo ThreadPoolDemo.cpp)
target_link_libraries(runThreadPoolDemo embeddedrtps lwip)

add_executable(lwipTests test_lwip.cpp)
target_link_libraries(lwipTests embeddedrtps lwip gtest gtest_main gmock gmock_main)

add_executable(latencyTest
        performance/LatencyTest.cpp
        performance/MeasurementUnit.cpp
        performance/ResponderUnit.cpp)
target_link_libraries(latencyTest embeddedrtps lwip)

add_executable(lwipLatencyTest
        lwip_performance/LwipLatencyTest.cpp
        lwip_performance/LwipMeasurementUnit.cpp
        lwip_performance/LwipResponderUnit.cpp)
target_link_libraries(lwipLatencyTest embeddedrtps lwip)

add_executable(fastddsComp
        fastdds-comp/CompTest.cpp
        fastdds-comp/Sender.cpp
        fastdds-comp/Receiver.cpp
        fastdds-comp/HelloWorld.c)
target_link_libraries(fastddsComp embeddedrtps lwip)

add_executable(MulticastPerformance
        multicast-performance/MulticastPerformance.cpp
        multicast-performance/McastSender.cpp
        multicast-performance/McastReceiver.cpp
        multicast-performance/MeasurementPacket.c)
target_link_libraries(MulticastPerformance embeddedrtps lwip)
