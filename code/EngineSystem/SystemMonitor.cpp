#include "SystemMonitor.h"

#include "imgui.h"

float SystemMonitor::m_CPUUsage = 0.0f;
float SystemMonitor::m_RAMUsage = 0.0f;
float SystemMonitor::m_RAMUsedGB = 0.0f;
float SystemMonitor::m_GPUUsage = 0.0f;
float SystemMonitor::m_VRAMUsage = 0.0f;
float SystemMonitor::m_VRAMUsedGB = 0.0f;
float SystemMonitor::m_VRAMTotalGB = 0.0f;

PDH_HQUERY SystemMonitor::m_Query = NULL;
PDH_HCOUNTER SystemMonitor::m_CpuCounter = NULL;
PDH_HCOUNTER SystemMonitor::m_MemCounter = NULL;
PDH_HCOUNTER SystemMonitor::m_GpuCounter = NULL;
PDH_HCOUNTER SystemMonitor::m_VRamUsedCounter = NULL;

void SystemMonitor::Initialize()
{
	PdhOpenQuery(NULL, NULL, &m_Query);

	PdhAddEnglishCounter(m_Query, "\\Processor(_Total)\\% Processor Time", NULL, &m_CpuCounter);
	PdhAddEnglishCounter(m_Query, "\\Memory\\% Committed Bytes In Use", NULL, &m_MemCounter);
	PdhAddEnglishCounter(m_Query, "\\GPU Engine(*)\\Utilization Percentage", NULL, &m_GpuCounter);
	PdhAddEnglishCounter(m_Query, "\\GPU Adapter Memory(*)\\Dedicated Usage", NULL, &m_VRamUsedCounter);

	// DXGIを使用してVRAMの総容量を取得（最も大きな専用VRAMを持つアダプタを選択）
	IDXGIFactory* pFactory = nullptr;
	if (SUCCEEDED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pFactory))) {
		IDXGIAdapter* pAdapter = nullptr;
		SIZE_T maxDedicatedMemory = 0;

		// 全てのアダプタを列挙して最大の専用VRAMを持つものを探す
		for (UINT i = 0; pFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND; ++i) {
			DXGI_ADAPTER_DESC adapterDesc;
			if (SUCCEEDED(pAdapter->GetDesc(&adapterDesc))) {
				// 専用ビデオメモリが最大のアダプタを選択（通常は専用GPU）
				if (adapterDesc.DedicatedVideoMemory > maxDedicatedMemory) {
					maxDedicatedMemory = adapterDesc.DedicatedVideoMemory;
					m_VRAMTotalGB = static_cast<float>(adapterDesc.DedicatedVideoMemory) / (1024.0f * 1024.0f * 1024.0f);
				}
			}
			pAdapter->Release();
		}

		pFactory->Release();
	}

	PdhCollectQueryData(m_Query);
	Sleep(1000); // 初回のデータ収集後、1秒待機してから再度収集することで正確な値を取得

}

void SystemMonitor::Update()
{
	PdhCollectQueryData(m_Query);
	DWORD counterType;
	PDH_FMT_COUNTERVALUE counterValue;


	PdhGetFormattedCounterValue(m_CpuCounter, PDH_FMT_DOUBLE, &counterType, &counterValue);
	m_CPUUsage = static_cast<float>(counterValue.doubleValue);


	PdhGetFormattedCounterValue(m_MemCounter, PDH_FMT_DOUBLE, &counterType, &counterValue);
	m_RAMUsage = static_cast<float>(counterValue.doubleValue);


	MEMORYSTATUSEX memStat = {};
	memStat.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memStat);

	m_RAMUsedGB = (memStat.ullTotalPhys - memStat.ullAvailPhys) / (1024.0f * 1024.0f * 1024.0f);


	PdhGetFormattedCounterValue(m_GpuCounter, PDH_FMT_DOUBLE, &counterType, &counterValue);
	m_GPUUsage = static_cast<float>(counterValue.doubleValue);
	

	// VRAM使用量(GB)
	PDH_FMT_COUNTERVALUE vramUsedValue;
	if (PdhGetFormattedCounterValue(m_VRamUsedCounter, PDH_FMT_LARGE, &counterType, &vramUsedValue) == ERROR_SUCCESS) {
		m_VRAMUsedGB = static_cast<float>(vramUsedValue.largeValue) / (1024.0f * 1024.0f * 1024.0f);
	}
	else {
		m_VRAMUsedGB = 0.0f;
	}

	// VRAM使用率(%)- 使用量と総容量から計算
	if (m_VRAMTotalGB > 0.0f) {
		m_VRAMUsage = (m_VRAMUsedGB / m_VRAMTotalGB) * 100.0f;
	}
	else {
		m_VRAMUsage = 0.0f;
	}
}

void SystemMonitor::Draw()
{
	ImGui::Text("CPU Usage: %.2f%%", m_CPUUsage);
	ImGui::Text("RAM Usage: %.2f%% (%.2f GB used)", m_RAMUsage, m_RAMUsedGB);
	ImGui::Text("GPU Usage: %.2f%%", m_GPUUsage);
	ImGui::Text("VRAM Usage: %.2f%% (%.2f GB / %.2f GB)", m_VRAMUsage, m_VRAMUsedGB, m_VRAMTotalGB);
}
