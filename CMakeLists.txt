cmake_minimum_required(VERSION 2.8)
project( HelloWorld )

# Add packages
find_package( OpenCV REQUIRED )
MESSAGE( STATUS "OPENCV_VERSION: " ${OpenCV_VERSION} )

# Sources
FILE(GLOB MY_SRC_FILES "*.cpp" "*.hpp")

# Compiler flags
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Build program
add_executable( hello ${MY_SRC_FILES})
target_link_libraries( hello ${OpenCV_LIBS})

