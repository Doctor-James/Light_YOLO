find_package(Threads)
set(Boost_INCLUDE_DIRS "/usr/include/boost")
find_package(Boost)
include_directories(
        inc
        ${Boost_INCLUDE_DIRS}
)

add_library(
        serialPort
        src/serialPort.cpp
)
target_link_libraries(
        serialPort
        -lboost_system
        -lpthread
)
