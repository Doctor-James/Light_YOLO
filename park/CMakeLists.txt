find_package(OpenCV REQUIRED)

include_directories(
        inc 
        ${OpenCV_INCLUDE_DIRS}
		"/usr/include/eigen3"
		../driver/camera/inc
		../driver/serialPort/inc
		../thread/inc
)
add_library(
        park
        src/park.cpp
		src/rotate.cpp
		src/rvec2elur.cpp
		src/boundarydetection.cpp
		src/transform.cpp
)
target_link_libraries(
		park
        ${OpenCV_LIBS}
)
