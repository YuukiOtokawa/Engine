#pragma once
#include "ObjectInterfacePerModule.h"

#include "RuntimeInclude.h"

#include <unordered_map>

#include "EngineAPI.h"

// すべてのファイルに認識されてRCCppが検出されるようにする
RUNTIME_MODIFIABLE_INCLUDE; //recompile runtime files when this changes

struct RCCppMainLoopI;
struct ImGuiContext;

struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11RenderTargetView;

struct ImDrawData;  

typedef void (*ImGui_ImplDX11_RenderDrawDataFunc)(ImDrawData* draw_data);

struct RuntimeObjectSystem;

static SystemTable*& g_pSystem = PerModuleInterface::g_pSystemTable;

struct OTOKAWA_API SystemTable
{
    RCCppMainLoopI* pRCCppMainLoopI = 0;

	ImGui_ImplDX11_RenderDrawDataFunc pImGui_ImplDX11_RenderDrawData = 0;

	RuntimeObjectSystem* pRuntimeObjectSystem = 0;

};
