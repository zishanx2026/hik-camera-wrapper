find_package(OpenCV REQUIRED)

set(LIB_SOURCES
    src/Camera.cpp
    src/sdk.cpp
)

include_directories(/opt/MVS/include)
link_directories(/opt/MVS/lib/64)

add_library(camera_lib SHARED ${LIB_SOURCES})
target_include_directories(camera_lib PUBLIC include)

target_link_libraries(camera_lib PUBLIC ${OpenCV_LIBS} /opt/MVS/lib/64/libMvCameraControl.so)