cmake_minimum_required(VERSION 3.10)
project(camera_app)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)

add_executable(camera_app main.cpp)



target_include_directories(camera_app PRIVATE /opt/MVS/include ${OpenCV_INCLUDE_DIRS})
target_link_libraries(camera_app PRIVATE camera_lib ${OpenCV_LIBS} /opt/MVS/lib/64/libMvCameraControl.so)