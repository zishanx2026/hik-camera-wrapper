#include <Camera.h>

#include <MvCameraControl.h>

#include <iostream>

int main()
{
    Camera cam;
    if (!cam.open(0))
    {
        std::cout << "Failed to open camera." << std::endl;
        return -1;
    }
    cv::Mat photo = cam.getframe();
    if (photo.empty())
    {
        std::cout << "Failed to capture photo." << std::endl;
        return -1;
    }
    cv ::imshow("Camera",photo);
    cv::waitKey(0);
    cam.close();
    return 0;
}