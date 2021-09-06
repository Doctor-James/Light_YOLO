cmake_minimum_required(VERSION 3.5)
project(Light)

set(CMAKE_CXX_STANDARD 11)
set(Sophus_INCLUDE_DIRS "/usr/local/include/sophus")
set(Sophus_LIBS "/usr/local/lib/libSophus.so")
set(CMAKE_CXX_FLAGS "${CMAKE_CXXFLAGS} -O0")
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)
add_definitions(-DBOOST_ERROR_CODE_HEADER_ONLY)
add_subdirectory(./park)
add_subdirectory(./driver)
find_package(OpenCV 3 REQUIRED)
find_package(CUDA REQUIRED)
link_directories(/usr/local/cuda/lib64)
link_directories(/usr/lib/arrch64-linux-gnu)
cuda_add_library(myplugins SHARED ./Yolo/yololayer.cu)
target_link_libraries(myplugins nvinfer cudart)
include_directories(   
        park/inc
 	driver/camera/inc
        driver/serialPort/inc
	thread/inc
	Yolo
        ${OpenCV_INCLUDE_DIRS}
        /usr/local/include/eigen3
        ${Sophus_INCLUDE_DIRS}
		/usr/local/cuda/include
		/usr/include/arrch64-linux-gnu
)
add_library(
		thread
		park/src/park.cpp
		park/src/boundarydetection.cpp
		park/src/transform.cpp
		driver/camera/src/camera.cpp
		driver/serialPort/src/serialPort.cpp
		thread/src/autodrivethread.cpp
		thread/src/serialPortReadWriteThread.cpp
		thread/src/YoloThread.cpp
		thread/src/thread.cpp
		thread/src/cameraThread.cpp
		Yolo/yolov5.cpp
		#Yolo/calibrator.cpp
)
add_executable(Light
        	main.cpp
		Light.cpp
        )
target_link_libraries(
		Light
		park
		thread
		nvinfer
		cudart
		myplugins
		${OpenCV_LIBS}
		${Sophus_LIBS}
		-lpthread -lm
        )
