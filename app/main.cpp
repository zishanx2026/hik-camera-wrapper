#include <Camera.h>
#include <sdk.h>
#include <opencv2/opencv.hpp>
#include <iostream>

int main()
{
    Sdk& sdk = Sdk::instance();
    Camera cam;
    if (!cam.scan())
    {
        std::cout << "Failed to scan." << std::endl;
        return -1;
    }
    if (!cam.connect())
    {
        std::cout << "Failed to open camera." << std::endl;
        return -1;
    }
    cam.set();
    cam.start_grab();
    // 如果开启了自动曝光那么手动曝光就会失效
    std::cout << "是否开启自动曝光?  answer:true or false ." << std::endl;
    bool able = false;
    std::cin >> able;
    float exposure_time = 5000;
    if (able == true )cam.set_auto_exposure(able);
    else
    {
        std::cout << "请输入自定义曝光时间：（毫秒）" << std::endl;
        std::cin >> exposure_time;
        cam.set_exposure_time(exposure_time);
    }
    std::cout << "是否开启增益？ answer:true or false ." << std::endl;
    bool gain_current = false;
    std::cin >> gain_current;
    if (gain_current == true)
    {
        std::cout << "输入增益值：" << std::endl;
        float gain_db;
        std::cin >> gain_db;
        cam.set_gain(gain_db);
    }
    std::cout << "是否开启白平衡？ answer:true or false ." << std::endl;
    bool enable = false;
    std::cin  >> enable;
    if (enable == true )cam.set_white_balance_auto(enable);
    // 循环获取图像并显示
    while (true)
    {
        cv::Mat frame;
        if (cam.getframe(frame))
        {
            cv::imshow("Camera", frame);
            
        }
        else
        {
            std::cout << "Failed to get frame."<< std::endl;
            cam.close();
            cv::waitKey(500);
            //  重新扫描并连接
            if (cam.scan() && cam.connect()) 
            {
                // 4. 重新配置参数并开始采集
                cam.set();
                cam.start_grab();
                std::cout << "Reconnected successfully!" << std::endl;
            }
            else
            {
                std::cerr << "Reconnect failed, retrying..." << std::endl;
            }
            // 5. 跳过本次循环，直接进入下一轮取图
            continue; 
        }

        // 按键交互
        int key = cv::waitKey(1);
        if (key == 'q') break;
        else if (key == '+')
        {
            exposure_time += 5000;
            cam.set_exposure_time(exposure_time);
            std::cout << "当前曝光： " << exposure_time << std::endl;
        }
        else if (key == '-')
        {
            exposure_time -= 5000;
            cam.set_exposure_time(exposure_time);
            std::cout << "当前曝光： " << exposure_time << std::endl;
        }
    }
    cam.close();
    sdk.release();
    return 0;
}