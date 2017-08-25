cmake_minimum_required(VERSION 2.8)
project( HelloWorld )

# Add packages
find_package(PkgConfig REQUIRED)
# OpenCV
find_package( OpenCV REQUIRED )
MESSAGE( STATUS "OPENCV_VERSION: " ${OpenCV_VERSION} )
# SLAMDunk
pkg_search_module(KALAMOS_CONTEXT REQUIRED kalamos-context)
MESSAGE( STATUS "KALAMOS_CONTEXT_INCLUDE_DIRS: " ${KALAMOS_CONTEXT_INCLUDE_DIRS} )
MESSAGE( STATUS "KALAMOS_CONTEXT_LIBRARIES: " ${KALAMOS_CONTEXT_LIBRARIES} )
MESSAGE( STATUS "KALAMOS_CONTEXT_LIBRARY_DIRS : " ${KALAMOS_CONTEXT_LIBRARY_DIRS} )
MESSAGE( STATUS "KALAMOS_CONTEXT_LDFLAGS : " ${KALAMOS_CONTEXT_LDFLAGS} )
MESSAGE( STATUS "KALAMOS_CONTEXT_CFLAGS : " ${KALAMOS_CONTEXT_CFLAGS} )
include_directories(${KALAMOS_CONTEXT_INCLUDE_DIRS})
link_directories(${KALAMOS_CONTEXT_LIBRARY_DIRS})

# Compiler flags
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Build program
# hello
add_executable(hello hello.cpp)
target_link_libraries(hello ${OpenCV_LIBS})
# frameL
add_executable(frameL frameL.cpp)
target_link_libraries(frameL ${OpenCV_LIBS} ${KALAMOS_CONTEXT_LIBRARIES})
# sgbm
add_executable(sgbm sgbm.cpp)
target_link_libraries(sgbm ${OpenCV_LIBS} ${KALAMOS_CONTEXT_LIBRARIES})
# calibrate
#add_executable(calibrate calibrate.cpp)
#target_link_libraries(calibrate ${OpenCV_LIBS} ${KALAMOS_CONTEXT_LIBRARIES})
# kalamoscapture
add_library(kalamoscapture STATIC kalamoscapture.cpp)
target_link_libraries(kalamoscapture ${OpenCV_LIBS} ${KALAMOS_CONTEXT_LIBRARIES})
# kalamoscapture_demo
add_executable(kalamoscapture_demo kalamoscapture_demo.cpp)
target_link_libraries(kalamoscapture_demo kalamoscapture ${OpenCV_LIBS} ${KALAMOS_CONTEXT_LIBRARIES})
