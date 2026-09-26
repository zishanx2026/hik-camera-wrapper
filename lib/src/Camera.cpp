#include "Camera.h"

#include <MvCameraControl.h>

#include <iostream>

//创建一个Camera对象的初始化
Camera::Camera()
{
    m_handle = nullptr;
    m_camera_index = 0;
    m_is_open = false;   
    m_packet_size = 0;
}

Camera::~Camera()
{
    close();
}

// 设置最佳包大小
bool Camera::set_optimal_packet_size(MV_CC_DEVICE_INFO*DeviceInfo)
{
    if(m_handle == nullptr)
    {
        std::cerr << "Camera is not open or handle is null!" << std::endl;
        return false;
    }
    // 是否为GigE相机
    if(DeviceInfo->nTLayerType == MV_GIGE_DEVICE)
    {
        int ret_size = MV_CC_GetOptimalPacketSize(m_handle);
        m_packet_size = static_cast<unsigned int>(ret_size);
        if (m_packet_size > 0)
        {
            int ret_set = MV_CC_SetIntValue(m_handle, "GevSCPSPacketSize", m_packet_size);
            if (ret_set != MV_OK)
            {
                std::cerr << "Set Packet Size fail! :" << ret_set << std::endl;
                return false;
            }
            return true;
        }
        else
        {
            std::cerr << "Get Optimal Packet Size fail! :" << ret_size << std::endl;
            return false;
        }
    }
    return false;
}

// 关闭相机句柄
void Camera::close_handle()
{
    if (m_handle != nullptr)
    {
        int ret_destroy = MV_CC_DestroyHandle(m_handle);
        if (ret_destroy != MV_OK)
        {
            std::cerr << "Destroy Handle fail! :" << ret_destroy << std::endl;
        }
        m_handle = nullptr;
    }
}

bool  Camera::open(int camera_index)
{
    // 初始化句柄为空,防止野指针
    m_handle = nullptr;
    // 检查相机状态，避免重复开机
    if (m_is_open)
    {
        std::cout << "Device Is Powered-on!" << std::endl;
        return false;
    }
    int ret_initial = MC_CC_Initialize()
    if (ret_initial != MV_OK)
    {
        std::cout << "initiallize fail" << std::endl;
        return false;
    }
    MV_CC_DEVICE_INFO_LIST stDeviceList = {0};
    int ret_enum = MV_CC_EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE, &stDeviceList);
    if (ret_enum != MV_OK)
    {
        std::cout << "Enum Devices fail! :" << ret_enum << std::endl;
        return false;
    }
    m_camera_index = camera_index;
    // 检查相机索引是否超出范围
    if (m_camera_index >= stDeviceList.nDeviceNum)
    {
        std::cerr << "Camera index out of range!" << std::endl;
        return false;
    }
    MV_CC_DEVICE_INFO*selected_device = stDeviceList.pDeviceInfo[m_camera_index];
    // 检查设备是否可访问
    if (!MV_CC_IsDeviceAccessible(selected_device, MV_ACCESS_Exclusive))
    {
        std::cout << "Device is not accessible! :" << std::endl;
        return false;
    }
   
    int ret_create = MV_CC_CreateHandle(&m_handle, stDeviceList.pDeviceInfo[m_camera_index]);
    if (ret_create !=MV_OK)
    {
        std::cout << "Create Handle fail! :" << ret_create << std::endl;
        return false;
    }
   
    int ret_open = MV_CC_OpenDevice(m_handle,MV_ACCESS_Exclusive,0);
    // 检查设备是否打开成功
    if (ret_open !=MV_OK)
    {
        close_handle();
        return false;
    }
    
    MV_CC_DEVICE_INFO CameraDeviceInfo = {0};
    // 获取设备信息
    int ret_get = MV_CC_GetDeviceInfo(m_handle, &CameraDeviceInfo);
    if (ret_get != MV_OK)
    {
        close_handle();
        std::cerr << "Get Device Info fail! :" << ret_get << std::endl;
        return false;
    }
    //网络设置最佳包大小
    if (!set_optimal_packet_size(&CameraDeviceInfo))
    {
        std::cerr << "Warning: Set optimal packet size failed!" << std::endl;
    }
    // 设置触发模式为连续采集
    int ret_trigger = MV_CC_SetEnumValue(m_handle, "TriggerMode", 0);
    if (ret_trigger != MV_OK)
    {
        close_handle();
        std::cerr << "Set Trigger Mode fail! :" << ret_trigger << std::endl;
        return false;
    }
    // 取灰度图
    int ret_set = MV_CC_SetEnumValue(m_handle, "PixelFormat", PixelType_Gvsp_Mono8);
    if (ret_set != MV_OK)
    {
        close_handle();
        std::cerr << "Set Pixel Format fail! :" << ret_set << std::endl;
        return false;
    }
    
    int ret_start = MV_CC_StartGrabbing(m_handle);
    if (ret_start != MV_OK)
    {
        close_handle();
        std::cerr << "Start Grabbing fail! :" << ret_start << std::endl;
        return false;
    }
    m_is_open = true;
    return true;
}

// 设置曝光
bool Camera::set_exposure_time(float exposure_time, int& error_code)
{
    if (!m_is_open || m_handle == nullptr)
    {
        error_code = -1;
        std::cerr << "Camera is not open or handle is null!" << std::endl;
        return false;
    }
    int ret_exposure = MV_CC_SetFloatValue(m_handle, "ExposureTime", exposure_time);
    if (ret_exposure != MV_OK)
    {
        error_code = ret_exposure;
        close_handle();
        std::cerr << "Set Exposure Time fail! :" << ret_exposure << std::endl;
        return false;
    }
    error_code = 0;
    return true;
}

// 设置增益
bool Camera::set_gain(float gain_db, int& error_code)
{
    if (!m_is_open || m_handle == nullptr)
    {
        error_code = -1;
        std::cerr << "Camera is not open or handle is null!" << std::endl;
        return false;
    }
    int ret_gain = MV_CC_SetFloatValue(m_handle, "Gain", gain_db);
    if (ret_gain != MV_OK)
    {
        error_code = ret_gain;
        close_handle();
        std::cerr << "Set Gain fail! :" << ret_gain << std::endl;
        return false;
    }
    error_code = 0;
    return true;
}

// 设置自动增益
bool Camera::set_auto_exposure(bool able, int& error_code)
{
    if (!m_is_open || m_handle == nullptr)
    {
        error_code = -1;
        std::cerr << "Camera is not open or handle is null!" << std::endl;
        return false;
    }
    int ret_auto_exposure = MV_CC_SetEnumValue(m_handle, "ExposureAuto", able ? 2 : 0);
    if (ret_auto_exposure != MV_OK)
    {
        error_code = ret_auto_exposure;
        close_handle();
        std::cerr << "Set Auto Exposure fail! :" << ret_auto_exposure << std::endl;
        return false;
    }
    error_code = 0;
    return true;
}

//
bool Camera::set_white_balance_auto(bool able, int& error_code)
{
    if (!m_is_open || m_handle == nullptr)
    {
        error_code = -1;
        std::cerr << "Camera is not open or handle is null!" << std::endl;
        return false;
    }
    int ret_white = MV_CC_SetEnumValue(m_handle, "BalanceWhiteAuto", able ? 2 : 0);
    if (ret_white != MV_OK)
    {
        error_code = ret_white;
        close_handle();
        std::cerr << "Set Auto White Balance fail! :" << ret_white << std::endl;
        return false;
    }
    error_code = 0;
    return true;
}

bool Camera::getframe(cv::Mat& frame, int& error_code)
{
    if (!m_is_open || m_handle == nullptr)
    {
        error_code = -1;
        std::cerr << "Camera is not open or handle is null!" << std::endl;
        return false;
    } 
    MV_FRAME_OUT photo_frame = {0};
    int ret_buffer = MV_CC_GetImageBuffer(m_handle, &photo_frame, 1000);
    if (ret_buffer != MV_OK || photo_frame.pBufAddr == nullptr)
    {
        error_code = ret_buffer;
        std::cerr << "Get Image fail! :" << ret_buffer << std::endl;
        return false;
    }
    // 将获取的图像数据转换为OpenCV的Mat格式
    MV_FRAME_OUT_INFO_EX PhotoInfo = photo_frame.stFrameInfo;
    cv::Mat frame_first(PhotoInfo.nHeight, PhotoInfo.nWidth, CV_8UC1, static_cast<void*>(photo_frame.pBufAddr));
    // 清除缓存区
    frame = frame_first.clone(); 
    error_code = 0;
    MV_CC_FreeImageBuffer(m_handle, &photo_frame);
    return true;
}

void Camera::close()
{
    if (m_handle == nullptr || !m_is_open)
    {
        std::cerr << "Camera is not open or handle is null!" << std::endl;
        return;
    }
    int ret_stop = MV_CC_StopGrabbing(m_handle);
    if (ret_stop != MV_OK)
    {
        std::cerr << "Stop Grabbing fail! :" << ret_stop << std::endl;
    }
    int ret_close = MV_CC_CloseDevice(m_handle);
    if (ret_close != MV_OK)
    {
        std::cerr << "Close Device fail! :" << ret_close << std::endl;
        return;
    }
    close_handle();
    int ret_final = MV_CC_Finalize();
    if (ret_final != MV_OK)
    {
        std::cerr << "Ending Fail !:" << ret_final << std::endl;
        return;
    }
    m_is_open = false;
}
