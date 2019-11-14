# project name
PROJECT(tool)
# requirement of cmake version
cmake_minimum_required(VERSION 2.8)

# set the directory of executable files
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

#add_definitions("-O2 -Wall -g -std=c++11")
add_definitions("-Wall -g -std=c++11")

# find required opencv
find_package(OpenCV REQUIRED)
# directory of opencv headers
include_directories(
    ${OpenCV_INCLUDE_DIRS} 
    ${PROJECT_SOURCE_DIR}/caffe/include
    )
# name of executable file and path of source file
add_executable(tool main.cpp caffe.cpp shareMemory.cpp)
# directory of opencv library
link_directories(${OpenCV_INCLUDE_DIRS})

# opencv libraries
target_link_libraries(
    ${PROJECT_NAME} 
    ${OpenCV_LIBS} 
    pthread
    ${PROJECT_SOURCE_DIR}/caffe/build/lib/libcaffe.so 
    /usr/lib/x86_64-linux-gnu/libglog.so  
    /usr/lib/x86_64-linux-gnu/libboost_system.so 
    # /usr/lib/aarch64-linux-gnu/libglog.so  
    # /usr/lib/aarch64-linux-gnu/libboost_system.so 
    )
    
ADD_DEFINITIONS(-DCPU_ONLY=1)
