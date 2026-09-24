find_package(OpenCV REQUIRED)

include_directories(/opt/MVS/include)
link_directories(/opt/MVS/lib/64)

add_library(camera_lib SHARED src/Camera.cpp)
target_include_directories(camera_lib PUBLIC include)
target_link_libraries(camera_lib PRIVATE ${OpenCV_LIBS} MvCameraControl)