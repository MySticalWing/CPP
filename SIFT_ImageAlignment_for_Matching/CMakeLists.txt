# cmake needs this line
cmake_minimum_required(VERSION 3.1)

# Enable C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# set(CMAKE_BUILD_TYPE "Relase")
set(CMAKE_BUILD_TYPE "Debug")

set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")
# Define project name
project(image_alignment_with_sift)
# Find OpenCV, you may need to set OpenCV_DIR variable
# to the absolute path to the directory containing OpenCVConfig.cmake file
# via the command line or GUI
find_package(OpenCV REQUIRED)
# include src 
include_directories(src)
# find all source file:
file(GLOB_RECURSE sourcefiles "src/*.cpp")
# file(GLOB mainfile ImageAlignmentTest.cpp)
file(GLOB mainfile real_time.cpp)
# file(GLOB mainfile main.cpp)



# Declare the executable target built from your sources
add_executable(a.out ${mainfile} ${sourcefiles})

# Link your application with OpenCV libraries
target_link_libraries(a.out ${OpenCV_LIBS})
