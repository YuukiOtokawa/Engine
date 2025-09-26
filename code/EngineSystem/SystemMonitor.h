#pragma once

class SystemMonitor {
    static float m_CPUUsage;
    static float m_RAMUsage;
    static float m_GPUUsage;
    static float m_VRAMUsage;

    static float m_CPUTemp;
    static float m_GPUTemp;

public:
    void Update();
    static float GetCPUUsage() { return m_CPUUsage; }
    static float GetRAMUsage() { return m_RAMUsage; }
    static float GetGPUUsage() { return m_GPUUsage; }
    static float GetVRAMUsage() { return m_VRAMUsage; }
    static float GetCPUTemp() { return m_CPUTemp; }
    static float GetGPUTemp() { return m_GPUTemp; }

};
