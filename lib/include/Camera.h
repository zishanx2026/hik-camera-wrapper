#pragma once

#include <opencv2/opencv.hpp>

#include <MvCameraControl.h>

class Camera
{
    private:
        

        Camera(const Camera&) = delete;
        Camera& operator=(const Camera&) = delete;

        void *m_handle;
        bool m_is_open;
        int m_camera_index;
        unsigned int m_packet_size;
        MV_CC_DEVICE_INFO_LIST m_stDeviceList;
        void printerror(const std::string& step, unsigned int nRet);
        bool check();
        void close_handle();
        
        bool set_optimal_packet_size();
        bool set_triggermode();
        bool set_pixelformat();
        
    public:
        Camera();
        ~Camera();

        bool scan();
        bool connect();
        bool set();
        bool start_grab();
        bool getframe(cv::Mat& frame);
        void close(); 
        
        bool set_exposure_time(float exposure_time);
        bool set_auto_exposure(bool able);
        bool set_white_balance_auto(bool able);
        bool set_gain(float gain_db);
};