#pragma once

#include <opencv2/opencv.hpp>

#include <MvCameraControl.h>

class Camera
{
    private:
        void *m_handle;
        bool m_is_open;
        int m_camera_index;
        unsigned int m_packet_size;
        MV_CC_DEVICE_INFO_LIST m_stDeviceList;
        bool check();
        void close_handle();
    public:
        Camera();
        ~Camera();
        Camera(const Camera&) = delete;
        Camera& operator=(const Camera&) = delete;

        bool scan()
        bool connect();
        bool start_grab();
        bool getframe(cv::Mat& frame);
        void close();

        bool set_optimal_packet_size();
        bool set_triggermode();
        bool set_pixelformat();
        bool set_exposure_time(float exposure_time);
        bool set_auto_exposure(bool able);
        bool set_white_balance_auto(bool able);
        bool set_gain(float gain_db);
     
};