find_package(Threads REQUIRED)
find_package(OpenCV REQUIRED)

include_directories(
        inc
        ${Boost_INCLUDE_DIRS}
        ${OpenCV_INCLUDE_DIRS}

)

add_library(
        camera
        src/camera.cpp
)
target_link_libraries(
        camera
        ${OpenCV_LIBS}
		#${PROJECT_SOURCE_DIR}/driver/camera/lib/dahen_lib/libgxiapi.so
        -lpthread
)
