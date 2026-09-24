#include <Camera.h>

#include <iostream>

int main()
{
    Camera cam;
    if (!cam.open(0))
    {
        std::cout << "Failed to open camera." << std::endl;
        return -1;
    }
    std::cout << "Open 成功，准备取图" << std::endl;
    // 循环获取图像并显示
    while (true)
    {
        int error_code = 0;
        cv::Mat frame;
        if (cam.getframe(frame, error_code))
        {
            cv::imshow("Camera", frame);
            // 按下 'q' 键退出
            if (cv::waitKey(1) == 'q') break;
        }
        else
        {
            std::cout << "Failed to get frame. Error code: " << error_code << std::endl;
        }
    }
    cam.close();
    return 0;
}