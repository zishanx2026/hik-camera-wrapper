#include <sdk.h>
#include <MvCameraControl.h>

#include <iostream>

Sdk& Sdk::instance()
{
    static Sdk s_instance;
    return s_instance;
}

bool Sdk::initial()
{
    if (m_initialized) return true; 

    int nRet = MV_CC_Initialize();
    if (nRet != MV_OK)
    {
        std::cerr << "[Sdk] Initialize failed! Ret: 0x"
                  << std::hex << nRet << std::dec << std::endl;
        return false;
    }
    m_initialized = true;
    return true;
}

void Sdk::release()
{
    if (!m_initialized) return;

    MV_CC_Finalize();
    m_initialized = false;
}


Sdk::Sdk()
{
    m_initialized = false;
}

Sdk::~Sdk()
{
    release();
}
