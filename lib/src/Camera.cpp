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
    m_stDeviceList = {0};
}

Camera::~Camera()
{
    close();
}

// 设置最佳包大小
bool Camera::set_optimal_packet_size()
{
    int nRet = MV_OK;
    if (!check())return false;
    MV_CC_DEVICE_INFO CameraDeviceInfo = {0};
    // 获取设备信息
    nRet = MV_CC_GetDeviceInfo(m_handle, &CameraDeviceInfo);
    if (nRet != MV_OK) 
    {
        printerror("Getdeviceinfo", nRet);
        return false;
    }
    // 是否为GigE相机
    if(CameraDeviceInfo.nTLayerType == MV_GIGE_DEVICE)
    {
        nRet = MV_CC_GetOptimalPacketSize(m_handle);
        if (nRet <= 0)
        {
            printerror("Get Optimal Packet Size", nRet);
            return false;
        }
        m_packet_size = static_cast<unsigned int>(nRet);
        nRet = MV_CC_SetIntValue(m_handle, "GevSCPSPacketSize", m_packet_size);
        if (nRet != MV_OK)
        {
            printerror("Set Packet Size", nRet);
            return false;
        }
    }
    return true;
}

// 关闭相机句柄
void Camera::close_handle()
{
    if (m_handle != nullptr)
    {
        int nRet = MV_CC_DestroyHandle(m_handle);
        if (nRet != MV_OK) printerror("Destroy Handle", nRet);
        m_handle = nullptr;
    }
}

bool Camera::check()
{
    if (!m_is_open || m_handle == nullptr)
    {
        std::cerr << "Camera is not open or handle is null!" << std::endl;
        return false;
    }
    else
    {
        return true;
    }
}

bool Camera::scan()
{
    if (m_handle != nullptr)close_handle(); 
    // 加快扫描速度
    MV_GIGE_SetEnumDevTimeout(50);

    // 带排序的枚举（按序列号，保证多相机稳定性）
    int nRet = MV_CC_EnumDevicesEx2(
        MV_GIGE_DEVICE | MV_USB_DEVICE,   // 想同时支持网口和USB
        &m_stDeviceList,
        nullptr,                           // 不需要按IP排序
        SortMethod_SerialNumber            // 按序列号排序
    );

    if (nRet != MV_OK)
    {
        PrintError("EnumDevicesEx2", nRet);
        return false;
    }

    std::cout << "Found " << m_stDeviceList.nDeviceNum << " camera(s)." << std::endl;
    return true;
}
bool Camera::connect()
{
    if (m_is_open == true)
    {
        std::cerr << "Don't Open Again !" << std::endl;
        return false;
    } 
    if (m_stDeviceList.nDeviceNum == 0)
    {
        std::cerr << "No device found, please call scan() first." << std::endl;
        return false;
    }
    // 检查相机索引是否超出范围
    if (m_camera_index >= m_stDeviceList.nDeviceNum)
    {
        std::cerr << "Camera index out of range!" << std::endl;
        return false;
    }
    int nRet = MV_OK;
    MV_CC_DEVICE_INFO*selected_device = m_stDeviceList.pDeviceInfo[m_camera_index];
    // 检查设备是否可访问
    if (!MV_CC_IsDeviceAccessible(selected_device, MV_ACCESS_Exclusive))
    {
        std::cout << "Device is not accessible! :" << std::endl;
        return false;
    } 
    nRet = MV_CC_CreateHandle(&m_handle, m_stDeviceList.pDeviceInfo[m_camera_index]);
    if (nRet !=MV_OK)
    {
        printerror("Create Handle", nRet);
        return false;
    }
    nRet = MV_CC_OpenDevice(m_handle,MV_ACCESS_Exclusive,0);
    // 检查设备是否打开成功
    if (nRet !=MV_OK)
    {
        printerror("OpenDevice", nRet);
        close_handle();
        return false;
    }
    m_is_open = true;
    return true;
}

// 设置触发模式为连续采集
bool Camera::set_triggermode()
{
    if (!check())return false;
    int nRet = MV_OK;
    nRet = MV_CC_SetEnumValue(m_handle, "TriggerMode", 0);
    if (nRet != MV_OK)
    {
        printerror("Trigger Mode", nRet);
        return false;
    }
    return true;
}

// 取灰度图
bool Camera::set_pixelformat()
{
    if (!check())return false;
    int nRet = MV_OK;
    nRet = MV_CC_SetEnumValue(m_handle, "PixelFormat", PixelType_Gvsp_Mono8);
    if (nRet != MV_OK)
    {
        printerror("PixelFormat", nRet);
        return false;
    }
    return true;
}

// 设置曝光
bool Camera::set_exposure_time(float exposure_time)
{
    if (!check())return false;
    int nRet = MV_OK;
    nRet = MV_CC_SetFloatValue(m_handle, "ExposureTime", exposure_time);
    if (nRet != MV_OK)
    {
        printerror("Set Exposure Time", nRet);
        return false;
    }
    return true;
}

// 设置增益
bool Camera::set_gain(float gain_db)
{
    if (!check())return false;
    int nRet = MV_OK;
    nRet = MV_CC_SetFloatValue(m_handle, "Gain", gain_db);
    if (nRet != MV_OK)
    {
        printerror("Set Gain", nRet);
        return false;
    }
    return true;
}

// 设置自动增益
bool Camera::set_auto_exposure(bool able)
{
    if (!check())return false;
    int nRet = MV_OK;
    nRet = MV_CC_SetEnumValue(m_handle, "ExposureAuto", able ? 2 : 0);
    if (nRet != MV_OK)
    {
        printerror("Set Auto Exposure", nRet);
        return false;
    }
    return true;
}

//
bool Camera::set_white_balance_auto(bool able)
{
    if (!check())return false;
    int nRet = MV_OK;
    nRet = MV_CC_SetEnumValue(m_handle, "BalanceWhiteAuto", able ? 2 : 0);
    if (nRet != MV_OK)
    {
        printerror("Set Auto White Balance", nRet);
        return false;
    }
    return true;
}

bool Camera::start_grab()
{
    int nRet = MV_OK;
    if (!check())return false;
    nRet = MV_CC_StartGrabbing(m_handle);
    if (nRet != MV_OK)
    {
        printerror("Start Grabbing", nRet);
        return false;
    }
    return true;
}

bool Camera::getframe(cv::Mat& frame)
{
    int nRet = MV_OK;
    if (!check())return false;
    MV_FRAME_OUT photo_frame = {0};
    nRet = MV_CC_GetImageBuffer(m_handle, &photo_frame, 1000);
    if (nRet != MV_OK)
    {
        printerror("GetImageBuffer", nRet);
        return false;
    }
    if (photo_frame.pBufAddr == nullptr)
    {
        std::cerr << "[GetImageBuffer] Returned null buffer." << std::endl;
        MV_CC_FreeImageBuffer(m_handle, &photo_frame); // 必须还回去！
        return false;
    }
    // 将获取的图像数据转换为OpenCV的Mat格式
    MV_FRAME_OUT_INFO_EX PhotoInfo = photo_frame.stFrameInfo;
    cv::Mat frame_first(PhotoInfo.nHeight, PhotoInfo.nWidth, CV_8UC1, static_cast<void*>(photo_frame.pBufAddr));
    // 清除缓存区
    frame = frame_first.clone(); 
    MV_CC_FreeImageBuffer(m_handle, &photo_frame);
    return true;
}

void Camera::close()
{
    int nRet = MV_OK;
    if (m_handle == nullptr) return; 
    nRet = MV_CC_StopGrabbing(m_handle);
    if (nRet != MV_OK)
    {
        printerror(" Stop Grabbing", nRet);
        nRet = MV_OK;
    }
    nRet = MV_CC_CloseDevice(m_handle);
    if (nRet != MV_OK)printerror("Close Device", nRet);
    close_handle();
    m_is_open = false;
}

void Camera::printerror(const std::string& step, int nRet)
{
    std::cerr << "[" << step << "] Failed! Ret: 0x" 
              << std::hex << nRet << std::dec;
              
    // 根据海康的错误码进行翻译
    switch (nRet)
    {
        case MV_E_HANDLE:
            std::cerr << " (Invalid handle or Error handle)";
            break;
        case MV_E_NODATA:
            std::cerr << " (No data available)";
            break;
        case MV_E_ACCESS_DENIED:
            std::cerr << " (Device is not accessible or occupied)";
            break;
        case MV_E_CALLORDER:
            std::cerr << " (Function call order error)";
            break;
        case MV_E_PARAMETER:
            std::cerr << " (Invalid parameter)";
            break;
        case MV_E_RESOURCE:
            std::cerr << " (Resource allocation failed)";
            break;
        default:
            std::cerr << " (Unknown error, see error code)";
            break;
    }
    std::cerr << std::endl;
}