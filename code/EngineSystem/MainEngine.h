// ========================================================
//
// エンジンシステム[MainEngine.h]
// 
//									Date:20250514
//									Author:Yuuki Otokawa
// ========================================================

#pragma once

//==========================================================================
// ヘッダーインクルード
//==========================================================================

#include "RenderCore.h"
#include "Editor.h"

#include "ObjectInterfacePerModule.h"

#include "EngineAPI.h"

#include "yaml.h"

#ifdef _DEBUG
#pragma comment(lib, "yaml-cppd.lib")
#else
#pragma comment(lib, "yaml-cpp.lib")
#endif // DEBUG


std::string OpenImportFileDialog();
std::string OpenExportFileDialog();


extern OTOKAWA_API SystemTable* g_pSystemTable;

//==========================================================================
// クラス定義
//==========================================================================

class Time;
class EngineConsole;
class SystemMonitor;
class ScriptDirectoryWatcher;

struct IObjectFactorySystem;


class OTOKAWA_API MainEngine
{
private:
	// エンジンメニューの定義
	enum EngineMenu {
		Editor_NewWindow = 1,
        Editor_Save,
        Editor_Load,
		Editor_End,
	};

	// レンダラーとエディターのポインタ
	RenderCore* m_pRenderer = nullptr;
	Editor* m_pEditor = nullptr;

	// ログファイルのポインタ
	FILE* m_pFile = nullptr;

	// FPSカウンタ変数
	static DWORD g_CountFPS;



	// メッセージ構造体
	MSG m_Message = {};

	//メインウィンドウハンドル
	HWND m_hWnd = NULL;

    Time* m_pTimeSystem = nullptr;

    EngineConsole* m_pEngineConsole = nullptr;
    
    SystemMonitor* m_pSystemMonitor = nullptr;

    ScriptDirectoryWatcher* m_pScriptWatcher = nullptr;

    IObjectFactorySystem* m_pObjectFactorySystem;

	// インスタンスハンドル
	HINSTANCE m_hInstance = NULL;


	//コンストラクタ
	MainEngine() = default;
	MainEngine(const MainEngine&) = delete;
	MainEngine& operator=(const MainEngine&) = delete;

	//デストラクタ
	~MainEngine() {
		
	}

	//クラスインスタンスポインタ
	static MainEngine* m_pInstance;

    bool RCCppInitialize();
    void RCCppUpdate();
    void RCCppFinalize();

public:

    /// @brief MainEngine クラスのシングルトンインスタンスを取得します。
	/// @return MainEngine クラスの唯一のインスタンスへのポインタ。
	static MainEngine* GetInstance() {
		if (m_pInstance == nullptr) {
			m_pInstance = new MainEngine();
		}
		return m_pInstance;
	}

	/// @brief システムのメインループを実行します。
	/// @return ループの終了時に返されるステータスコード（整数値）。
	int SystemLoop();

	/// @brief アプリケーションの初期化を行います。
	/// @param hInstance 現在のアプリケーションインスタンスのハンドル。
	/// @param hPrevInstance 前回のアプリケーションインスタンスのハンドル（常にNULL）。
	/// @param lpCmdLine コマンドライン引数の文字列。
	/// @param nCmdShow ウィンドウの表示状態を指定するフラグ。
	/// @return 初期化が成功した場合は0、失敗した場合は0以外の値を返します。
	int Initialize(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
	/// @brief オブジェクトやリソースの後処理を行います。
	void Finalize();

	/// @brief ウィンドウプロシージャとして、ウィンドウメッセージを処理します。
	/// @param hWnd メッセージを受信するウィンドウのハンドル。
	/// @param message 処理するメッセージの識別子。
	/// @param wParam メッセージに関連する追加情報（ワードパラメータ）。
	/// @param lParam メッセージに関連する追加情報（ロングパラメータ）。
	/// @return メッセージの処理結果を示す値。
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	/// @brief ウィンドウプロシージャとして、ウィンドウメッセージを処理します。
	/// @param hWnd メッセージを受け取るウィンドウのハンドル。
	/// @param message 処理するメッセージの識別子。
	/// @param wParam メッセージに関連する追加情報（ワード単位）。
	/// @param lParam メッセージに関連する追加情報（ロング単位）。
	/// @return メッセージの処理結果。特定のメッセージに応じた値を返します。
	LRESULT Proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	/// @brief ウィンドウのハンドルを取得します。
	/// @return ウィンドウのハンドル（HWND型）を返します。
	HWND GetWindow() { return m_hWnd; }
	/// @brief インスタンスハンドルを取得します。
	/// @return 現在のインスタンスのハンドル（HINSTANCE型）を返します。
	HINSTANCE GetInstanceHandle() { return m_hInstance; }
	/// @brief レンダラーオブジェクトを取得します。
	/// @return m_pRenderer へのポインタ。
	RenderCore* GetRenderCore() { return m_pRenderer; }

	static void ErrorMessage(std::string message, std::string title);

	void GetWindowsInfo();


    static INT_PTR CALLBACK FilePathDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

};

