CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

set(CMAKE_CXX_STANDARD 11)

SET(MODULE_NAME messageQueue)

INCLUDE_DIRECTORIES(
        inc
#        test/
)


file(GLOB_RECURSE CURRENT_HEADERS inc/*.h  inc/*.hpp )

file(GLOB CURRENT_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c  )
message("${CURRENT_SOURCES}::::::::::::::::::::")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden  ")
set(CMAKE_CXX_FLAGS "-std=c++11 -pedantic -Wall -Wextra -Weffc++ -O3")
ADD_LIBRARY( ${MODULE_NAME} STATIC
        ${CURRENT_HEADERS} ${CURRENT_SOURCES}
)

TARGET_LINK_LIBRARIES(${MODULE_NAME} "pthread" )
#add_executable(testpolym test/Test.cpp )
#target_link_libraries(testpolym  ${MODULE_NAME} )
#target_link_libraries(testpolym  "pthread"  "opencv_core" "opencv_highgui" "opencv_imgcodecs" "opencv_imgproc")
