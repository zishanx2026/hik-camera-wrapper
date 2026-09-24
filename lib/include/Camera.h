#pragma once

#include <opencv2/opencv.hpp>

#include <MvCameraControl.h>

class Camera
{
    private:
        void *m_handle;
        bool m_is_open;
        int m_camera_index;
        bool set_optimal_packet_size(MV_CC_DEVICE_INFO *DeviceInfo,void *handle);
        unsigned int m_packet_size;
        void close_handle();
    public:
        Camera();
        ~Camera();
        bool open(int m_camera_index);
        cv::Mat getframe();
        void close();
};