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
    // 设置初始参数
    int error_set = 0;
    // 如果开启了自动曝光那么手动曝光就会失效
    std::cout << "是否开启自动曝光?  answer:true or false ." << std::endl;
    bool able = false;
    std::cin >> able;
    float exposure_time = 5000;
    if (able == true )cam.set_auto_exposure(able, error_set);
    else
    {
        std::cout << "请输入自定义曝光时间：（毫秒）" << std::endl;
        std::cin >> exposure_time;
        cam.set_exposure_time(exposure_time, error_set);
    }
    std::cout << "是否开启增益？ answer:true or false ." << std::endl;
    bool gain_current = false;
    std::cin >> gain_current;
    if (gain_current == true)
    {
        std::cout << "输入增益值：" << std::endl;
        float gain_db;
        std::cin >> gain_db;
        cam.set_gain(gain_db, error_set);
    }
    std::cout << "是否开启白平衡？ answer:true or false ." << std::endl;
    bool enable = false;
    std::cin  >> enable;
    if (able == true )cam.set_white_balance_auto(enable, error_set);
    // 循环获取图像并显示
    while (true)
    {
        int error_code = 0;
        cv::Mat frame;
        if (cam.getframe(frame, error_code))
        {
            cv::imshow("Camera", frame);
            
        }
        else
        {
            std::cout << "Failed to get frame. Error code: " << error_code << std::endl;
        } 
        int key = cv::waitKey(1);
        if (key == 'q')break;
        else if (key == '+')
        {
            exposure_time += 5000;
            cam.set_exposure_time(exposure_time, error_code);
            std::cout << "当前曝光： " << exposure_time << std::endl;
        }
        else if (key == '-')
        {
            exposure_time -= 5000; 
            cam.set_exposure_time(exposure_time, error_code);
            std::cout << "当前曝光： " << exposure_time << std::endl;
        }
    }
    cam.close();
    return 0;
}