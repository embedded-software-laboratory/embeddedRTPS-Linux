cmake_minimum_required(VERSION 3.10)

add_subdirectory(thirdparty/googletest)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
include_directories(${rtps_SOURCE_DIR}/include ${rtps_SOURCE_DIR})


#add_executable(runThreadPoolDemo ThreadPoolDemo.cpp)
#target_link_libraries(runThreadPoolDemo embeddedrtps lwip)

add_executable(lwipTests test_lwip.cpp)
target_link_libraries(lwipTests embeddedrtps lwip gtest gtest_main gmock gmock_main)

add_executable(fastddsComp
        fastdds-comp/CompTest.cpp
        fastdds-comp/Sender.cpp
        fastdds-comp/Receiver.cpp
        fastdds-comp/HelloWorld.c)
target_link_libraries(fastddsComp embeddedrtps lwip microcdr)

#add_executable(MulticastPerformance
#        multicast-performance/MulticastPerformance.cpp
#        multicast-performance/McastSender.cpp
#        multicast-performance/McastReceiver.cpp
#        multicast-performance/MeasurementPacket.c)
#target_link_libraries(MulticastPerformance embeddedrtps lwip)
