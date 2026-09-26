#pragma once

#include <opencv2/opencv.hpp>

#include <MvCameraControl.h>

class Camera
{
    private:
        void *m_handle;
        bool m_is_open;
        int m_camera_index;
        bool set_optimal_packet_size(MV_CC_DEVICE_INFO *DeviceInfo);
        unsigned int m_packet_size;
        void close_handle();
    public:
        Camera();
        ~Camera();
        Camera(const Camera&) = delete;
        Camera& operator=(const Camera&) = delete;

        bool open(int m_camera_index);
        bool getframe(cv::Mat& frame, int& error_code);
        void close();

        bool set_exposure_time(float exposure_time, int& error_code);
        bool set_auto_exposure(bool able, int& error_code);
        bool set_white_balance_auto(bool able, int& error_code);
        bool set_gain(float gain_db, int& error_code);
};