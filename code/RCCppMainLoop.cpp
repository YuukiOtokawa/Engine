#include "ObjectInterfacePerModule.h"
#include "IObject.h"
#include "RCCppMainLoop.h"

#include "SystemTable.h"

#include "imgui.h"


#include "RuntimeLinkLibrary.h"
RUNTIME_COMPILER_LINKLIBRARY("d3d11.lib");
#include <d3d11.h>
#define DIRECTINPUT_VERSION 0x0800

// add imgui source dependencies
// an alternative is to put imgui into a library and use RuntimeLinkLibrary
#include "RuntimeSourceDependency.h"
RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("imgui/imgui", ".cpp");
RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("imgui/imgui_widgets", ".cpp");
RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("imgui/imgui_draw", ".cpp");
RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("imgui/imgui_demo", ".cpp");

#include "RuntimeObjectSystem.h"
#include "IObjectFactorySystem.h"

// RCC++ uses interface Id's to distinguish between different classes
// here we have only one, so we don't need a header for this enum and put it in the same
// source code file as the rest of the code
// クラスを識別
enum InterfaceIDEnumConsoleExample
{
    IID_IRCCPP_MAIN_LOOP = IID_ENDInterfaceID, // IID_ENDInterfaceID from IObject.h InterfaceIDEnum

    IID_ENDInterfaceIDEnumConsoleExample
};
/*
TInterfaceから派生
    クラス識別enum
    RCCppの基本クラス IObject
*/
struct RCCppMainLoop : RCCppMainLoopI, TInterface<IID_IRCCPP_MAIN_LOOP, IObject>
{
    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    int presentInterval = 0;

    IObjectFactorySystem* factory;

    RCCppMainLoop() {
		PerModuleInterface::g_pSystemTable->pRCCppMainLoopI = this;
        factory = g_pSystem->pRuntimeObjectSystem->GetObjectFactorySystem();

    }
};

// メインクラスのパラメータと自動的に構築するかどうかを指定
// このマクロでRCCppが指定したクラスのシングルトンクラスを生成してくれる
REGISTERSINGLETON(RCCppMainLoop, true);