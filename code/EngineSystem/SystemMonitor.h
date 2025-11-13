#pragma once

#include <Pdh.h>
#include <PdhMsg.h>
#pragma comment(lib, "pdh.lib")

#include <Windows.h>
#include <dxgi.h>
#pragma comment(lib, "dxgi.lib")

class SystemMonitor {
    static float m_CPUUsage;
    static float m_RAMUsage;
    static float m_RAMUsedGB;
    static float m_GPUUsage;
    static float m_VRAMUsage;
    static float m_VRAMUsedGB;
    static float m_VRAMTotalGB;

    static PDH_HQUERY m_Query;
    static PDH_HCOUNTER m_CpuCounter;
    static PDH_HCOUNTER m_MemCounter;

    static PDH_HCOUNTER m_GpuCounter;
    static PDH_HCOUNTER m_VRamUsedCounter;
    
public:
    static void Initialize();
    static void Update();

    static void Draw();

    static float GetCPUUsage() { return m_CPUUsage; }
    static float GetRAMUsage() { return m_RAMUsage; }
    static float GetRAMUsedGB() { return m_RAMUsedGB; }
    static float GetGPUUsage() { return m_GPUUsage; }
    static float GetVRAMUsage() { return m_VRAMUsage; }
    static float GetVRAMUsedGB() { return m_VRAMUsedGB; }
};
