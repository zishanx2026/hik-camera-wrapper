#pragma once
#include <MvCameraControl.h>

class Sdk
{
    public:
        static Sdk& instance();
        bool initial();
        void release();
    private:
        Sdk();
        ~Sdk();

        Sdk(const Sdk&) = delete;
        Sdk& operator=(const Sdk&) = delete;

        bool m_initialized;
};