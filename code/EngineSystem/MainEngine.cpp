// ========================================================
//
// エンジンシステム[MainEngine.cpp]
// 
//									Date:20250514
//									Author:Yuuki Otokawa
// ========================================================

//==========================================================================
// ヘッダーインクルード
//==========================================================================

#include "MainEngine.h"
#include "Editor.h" // Editorをインクルード
#include "Node/Node.h" // ObjectNodeの定義を含むNode.hをインクルード

#include <sstream>

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
//#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)

#include <Psapi.h>

#include "FrameWork.h"

#include "Component_InputSystem.h"

#include <Windows.h>

#include "CSVExporter.h"
#include "SceneExporter.h"
#include "CSVImporter.h"
#include "SceneImporter.h"
#include "resource.h"
#include <Richedit.h>

#include "SystemLog.h"

#include "TimeSystem.h"
#include "EngineConsole.h"
#include "SystemMonitor.h"

#include "RuntimeObjectSystem.h"
#include "IObjectFactorySystem.h"
#include "StdioLogSystem.h"
#include "SystemTable.h"

#include "ScriptFactory.h"
#include "ScriptDirectoryWatcher.h"


constexpr auto WINDOW_CREATE_FAILED = -1;

MainEngine* MainEngine::m_pInstance = nullptr;


static StdioLogSystem g_Logger;
OTOKAWA_API SystemTable* g_pSystemTable = new SystemTable();

//==========================================================================
// メンバ関数
//==========================================================================

int APIENTRY WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	auto mainEngine = MainEngine::GetInstance();

	mainEngine->Initialize(hInstance, hPrevInstance, lpCmdLine, nCmdShow);

	auto param = mainEngine->SystemLoop();

	mainEngine->Finalize();

	return param;
}


bool MainEngine::RCCppInitialize()
{
	g_pSystemTable->pRuntimeObjectSystem = new RuntimeObjectSystem;

	if (!g_pSystemTable->pRuntimeObjectSystem->Initialise(&g_Logger, g_pSystemTable)) {
		delete g_pSystemTable->pRuntimeObjectSystem;
		g_pSystemTable->pRuntimeObjectSystem = NULL;
		ErrorMessage("RuntimeObjectSystemの初期化に失敗しました。", "エラー");
		return false;
	}

	m_pObjectFactorySystem = g_pSystemTable->pRuntimeObjectSystem->GetObjectFactorySystem();

	FileSystemUtils::Path basePath = g_pSystemTable->pRuntimeObjectSystem->FindFile(__FILE__).ParentPath();
	FileSystemUtils::Path imguiIncludeDir = basePath.ParentPath() / "imgui";
	FileSystemUtils::Path scriptIncludeDir = basePath.ParentPath().ParentPath() / "scripts";
	FileSystemUtils::Path dataManagerDir = basePath.ParentPath() / "DataManager";
	FileSystemUtils::Path componentDir = basePath.ParentPath() / "components";
	FileSystemUtils::Path directXTex = basePath.ParentPath().ParentPath() / "DirectXTex";
	FileSystemUtils::Path externalDir = basePath.ParentPath().ParentPath() / "external";
	FileSystemUtils::Path code = basePath.ParentPath();
	FileSystemUtils::Path engineSystem = basePath.ParentPath() / "EngineSystem";
	FileSystemUtils::Path input = basePath.ParentPath() / "input";
	FileSystemUtils::Path yaml = basePath.ParentPath() / "yaml-cpp-master" / "include" / "yaml-cpp";
	g_pSystemTable->pRuntimeObjectSystem->AddIncludeDir(basePath.c_str());
	g_pSystemTable->pRuntimeObjectSystem->AddIncludeDir(externalDir.c_str());
	g_pSystemTable->pRuntimeObjectSystem->AddIncludeDir(imguiIncludeDir.c_str());
	g_pSystemTable->pRuntimeObjectSystem->AddIncludeDir(scriptIncludeDir.c_str());
	g_pSystemTable->pRuntimeObjectSystem->AddIncludeDir(dataManagerDir.c_str());
	g_pSystemTable->pRuntimeObjectSystem->AddIncludeDir(directXTex.c_str());
	g_pSystemTable->pRuntimeObjectSystem->AddIncludeDir(componentDir.c_str());
	g_pSystemTable->pRuntimeObjectSystem->AddIncludeDir(code.c_str());
	g_pSystemTable->pRuntimeObjectSystem->AddIncludeDir(engineSystem.c_str());
	g_pSystemTable->pRuntimeObjectSystem->AddIncludeDir(input.c_str());
	g_pSystemTable->pRuntimeObjectSystem->AddIncludeDir(yaml.c_str());
	g_pSystemTable->pRuntimeObjectSystem->AddIncludeDir(yaml.ParentPath().c_str());

	g_pSystemTable->pRuntimeObjectSystem->AddLibraryDir("build/x64/Debug");
	g_pSystemTable->pRuntimeObjectSystem->AddLibraryDir("code/yaml-cpp-master/build/Debug");
	g_pSystemTable->pRuntimeObjectSystem->AddLibraryDir("code/yaml-cpp-master/build/Release");
#ifdef _DEBUG
	g_pSystemTable->pRuntimeObjectSystem->AddLibraryDir("code/DirectXTex-main/DirectXTex/Bin/Desktop_2022/x64/Debug");
#else
	g_pSystemTable->pRuntimeObjectSystem->AddLibraryDir("code/DirectXTex-main/DirectXTex/Bin/Desktop_2022/x64/Release");
#endif // _DEBUG


	
#ifdef _DEBUG
	// 動的ランタイムライブラリを明示的に除外し、必要なライブラリのみリンク
	g_pSystemTable->pRuntimeObjectSystem->SetAdditionalLinkOptions(
		" OtokawaEngine.lib"
		" yaml-cppd.lib"
		" DirectXTex.lib"
	);
#else
	g_pSystemTable->pRuntimeObjectSystem->SetAdditionalLinkOptions(
		" OtokawaEngine.lib"
		" yaml-cpp.lib"
		" DirectXTex.lib"
	);
#endif

	// 保留されたスクリプト登録を実行
	ScriptFactory::RegisterAllPendingScripts();

	// ScriptFactoryをホットリロードのリスナーとして登録
	g_pSystemTable->pRuntimeObjectSystem->GetObjectFactorySystem()->AddListener(&ScriptFactory::GetInstance());

	// scriptsフォルダの監視を開始
	m_pScriptWatcher = new ScriptDirectoryWatcher();
	m_pScriptWatcher->Initialize(scriptIncludeDir.c_str(), g_pSystemTable->pRuntimeObjectSystem);

	return true;
}

void MainEngine::RCCppUpdate()
{
	//check status of any compile
	if (g_pSystemTable->pRuntimeObjectSystem->GetIsCompiledComplete())
	{
		// save state
		// load module when compile complete
		g_pSystemTable->pRuntimeObjectSystem->LoadCompiledModule();

		// load state
	}

	if (!g_pSystemTable->pRuntimeObjectSystem->GetIsCompiling())
	{
		float deltaTime = 1.0f / ImGui::GetIO().Framerate;
		g_pSystemTable->pRuntimeObjectSystem->GetFileChangeNotifier()->Update(deltaTime);

		// scriptsフォルダの構造変化を処理
		if (m_pScriptWatcher) {
			m_pScriptWatcher->Update();
		}
	}
}

void MainEngine::RCCppFinalize()
{
	// scriptsフォルダ監視を終了
	if (m_pScriptWatcher) {
		m_pScriptWatcher->Finalize();
		delete m_pScriptWatcher;
		m_pScriptWatcher = nullptr;
	}

	delete g_pSystemTable->pRuntimeObjectSystem;
}

int MainEngine::SystemLoop()
{
	while (WM_QUIT != m_Message.message) {

		if (PeekMessage(&m_Message, NULL, 0, 0, PM_REMOVE)) {
			// メッセージがある場合はメッセージ処理を優先
			TranslateMessage(&m_Message);
			DispatchMessage(&m_Message);
		}
		else {
			RCCppUpdate();
			m_pTimeSystem->Update();
			if ((Time::currentTime - Time::lastSystemTime) >= std::chrono::duration<double>(1.0)) // 1秒ごとに実行
			{
				SystemMonitor::Update();
				m_pTimeSystem->SystemTimeUpdate();
			}
			if ((Time::currentTime - Time::lastFrameTime) >= Time::targetFrameDuration) // 1/60秒ごとに実行
			{
				m_pTimeSystem->FrameUpdate();

				std::stringstream caption;
				caption << WINDOW_CAPTION << " FPS: " << Time::FramePerSecond();
				SetWindowTextA(m_hWnd, caption.str().c_str());

				// ここがエンジン処理
				m_pEditor->Update();
				m_pEditor->Draw();

			}
			else {
				Sleep(0);
			}
		}
	}
	return (int)m_Message.wParam;
}

int MainEngine::Initialize(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// ウィンドウクラス構造体の設定
	WNDCLASS wc = {};
	wc.lpfnWndProc = WindowProc;                          // ウィンドウプロシージャの指定
	wc.lpszClassName = WINDOW_CLASS_NAME;                     // クラス名の設定
	wc.hInstance = m_hInstance = hInstance;                          // インスタンスハンドルの指定
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);          // マウスカーソルを指定
	wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND + 1); // ウインドウのクライアント領域の背景色を設定

	// クラス登録
	RegisterClass(&wc);

	//DWORD windowStyle = (WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME) ^ WS_MAXIMIZEBOX;
	//DWORD windowStyle = WS_OVERLAPPEDWINDOW | WS_MAXIMIZEBOX | WS_THICKFRAME;
	DWORD windowStyle = WS_OVERLAPPEDWINDOW;

	// 基本矩形座標
	RECT windowRect = { 0, 0, SCREEN_WIDTH_DEFAULT, SCREEN_HEIGHT_DEFAULT };

	// 指定したクライアント領域を確保するために新たな矩形座標を計算
	AdjustWindowRect(&windowRect, windowStyle, FALSE);

	// 新たなWindowの矩形座標から幅と高さを算出
	XMINT2 windowSize = { windowRect.right - windowRect.left, windowRect.bottom - windowRect.top };

	// プライマリモニターの画面解像度取得
	XMINT2 desktopSize = { GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };

	// タスクバーの高さを考慮して、描画領域の高さを調整
	RECT workArea;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);

	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	int taskbarHeight = screenHeight - workArea.bottom;


	// デスクトップの真ん中にウィンドウが生成されるように座標を計算
	// ※ただし万が一、デスクトップよりウィンドウが大きい場合は左上に表示
	XMINT2 windowPos = {
		desktopSize.x - windowSize.x >= 0 ? desktopSize.x - windowSize.x : 0,
		desktopSize.x - windowSize.x >= 0 ? desktopSize.y - windowSize.y : 0
	};

	// ウィンドウの生成
	m_hWnd = CreateWindowA(
		WINDOW_CLASS_NAME,          // ウィンドウ クラス
		WINDOW_CAPTION,      // ウィンドウ テキスト
		windowStyle,        // ウィンドウ スタイル

		// サイズと位置
		0,
		0,
		windowSize.x,
		windowSize.y,

		NULL,       // 親ウィンドウハンドル
		NULL,       // メニューハンドル
		hInstance,  // インスタンスハンドル
		NULL        // 追加のアプリケーションデータ
	);

	if (m_hWnd == NULL) {
		// ウィンドウハンドルが何らかの理由で生成出来なかった
		return WINDOW_CREATE_FAILED;
	}


	//メニューバーの設定
	HMENU hMenu = LoadMenu(m_hInstance, MAKEINTRESOURCE(IDR_MENU1));
	SetMenu(m_hWnd, hMenu);

	SetWindowPos(
		m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
	LoadLibrary("Riched20.dll"); // リッチエディットコントロールの読み込み

	SetWindowLongPtr(m_hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

//==========================================================================
// システム初期化
//==========================================================================

	// COMライブラリの初期化
	HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);

	// レンダラーの初期化
	m_pRenderer = new RenderCore(m_hWnd);

	// エディターの初期化
	m_pEditor = Editor::GetInstance();

	// 時間管理システムの初期化
	m_pTimeSystem = new Time;

	// コンソールウィンドウの初期化
	m_pEngineConsole = new EngineConsole;

	m_pSystemMonitor = new SystemMonitor;
	SystemMonitor::Initialize();

	RCCppInitialize();

	// ログコンソールの初期化
	AllocConsole();
	freopen_s(&m_pFile, "CONOUT$", "w", stdout);

	//imGUI初期化
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(m_hWnd);
	ImGui_ImplDX11_Init(
		m_pRenderer->GetDevice(),
		m_pRenderer->GetDeviceContext());


	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	ImGuiStyle& style = ImGui::GetStyle();
	style.ItemSpacing.y = 8.0f;

	// 指定のウィンドウハンドルのウィンドウを指定の方法で表示
	ShowWindow(m_hWnd, SW_MAXIMIZE);

	timeBeginPeriod(1); // 分解能を設定

	m_pTimeSystem->Reset();

	m_pEditor->Initialize();

	return 0;

}

void MainEngine::Finalize()
{
	m_pEditor->Finalize();
	delete m_pEditor;
	m_pEditor = nullptr;
	delete m_pRenderer;
	m_pRenderer = nullptr;
	if (m_pFile) {
		fclose(m_pFile);
		m_pFile = nullptr;
	}

	RCCppFinalize();

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
}

LRESULT MainEngine::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	MainEngine* pThis = reinterpret_cast<MainEngine*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	if (pThis) {
		//インスタンスが必要な場合、関数を呼ぶ
		return pThis->Proc(hWnd, message, wParam, lParam);
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT MainEngine::Proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;


	switch (message) {
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_FILE_ADDOBJECT: // オブジェクト追加
			new Object();
			break;
		case ID_FILE_SAVE: // 保存
			SceneExporter::Export(*m_pEditor->GetObjects());
			break;
		case ID_FILE_LOAD: // 読み込み
		{
			auto filePath = OpenImportFileDialog();
			if (filePath != "") {
				Editor::GetInstance()->OpenScene(filePath);
			}
		}
			break;
		case ID_FILE_EXIT: // 終了
			if (MessageBoxA(hWnd, "本当に終了してよろしいですか？", "確認", MB_OKCANCEL | MB_DEFBUTTON2) == IDOK) {
				DestroyWindow(hWnd); // 指定のウィンドウにWM_DESTROYメッセージを送る
			}
			return 0; // DefWindowProc関数にメッセージを流さず終了することによって何もなかったことにする
		case ID_WINDOW_NEWWINDOW:
		{
			GUI::GetInstance()->SetNodeEditorVisible();
            // テスト用のObjectNodeを追加
            //Editor::GetInstance()->GetNodeManager()->AddNode(new ObjectNode());
		}
		default:
			break;
		}
		break;

	case WM_CLOSE:
		if (MessageBoxA(hWnd, "本当に終了してよろしいですか？", "確認", MB_OKCANCEL | MB_DEFBUTTON2) == IDOK) {
			DestroyWindow(hWnd); // 指定のウィンドウにWM_DESTROYメッセージを送る
		}
		return 0; // DefWindowProc関数にメッセージを流さず終了することによって何もなかったことにする

	case WM_DESTROY: // ウィンドウの破棄メッセージ
		PostQuitMessage(0); // WM_QUITメッセージの送信
		return 0;
	case WM_SIZE:
		if (wParam == SIZE_MINIMIZED) {
			// ウィンドウが最小化された場合
			m_pRenderer->ResizeClient(0, 0);
		}
		else {
			// ウィンドウのサイズ変更
			m_pRenderer->ResizeClient(LOWORD(lParam), HIWORD(lParam));
		}
		break;

	case WM_ACTIVATEAPP:
		Keyboard::GetInstance()->KeyboardProcessMessage(message, wParam, lParam);
		break;

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) {
			SendMessage(hWnd, WM_CLOSE, 0, 0); // WM_CLOSEメッセージの送信
		}


	case WM_INPUT:
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_MOUSEHOVER:
		Mouse::GetInstance(hWnd)->ProcessMessage(message, wParam, lParam);

	case WM_KEYUP:
	case WM_SYSKEYUP:
	case WM_SYSKEYDOWN:
		Keyboard::GetInstance()->KeyboardProcessMessage(message, wParam, lParam);
		break;
	};

	// 通常メッセージ処理はこの関数に任せる
	return DefWindowProc(hWnd, message, wParam, lParam);

}

void MainEngine::ErrorMessage(std::string message, std::string title)
{
	HWND hWnd = GetInstance()->GetWindow();
	MessageBoxA(hWnd, message.c_str(), title.c_str(), MB_OK | MB_DEFBUTTON2);
	DestroyWindow(hWnd); // 指定のウィンドウにWM_DESTROYメッセージを送る
}

void MainEngine::GetWindowsInfo()
{
	// システムメモリ情報を取得
	MEMORYSTATUSEX statex = { sizeof(statex) };
	statex.dwLength = sizeof(statex);
	GlobalMemoryStatusEx(&statex);


	SIZE_T memory;

	// プロセスメモリ情報を取得(現在実行中のプロセス)
	PROCESS_MEMORY_COUNTERS_EX pmc = { sizeof(pmc) };
	if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
		memory = pmc.WorkingSetSize / 1024 / 1024 / 1024; // GB単位に変換
	}
	else {
		memory = 0;
	}

	// システム情報を取得 プロセッサとかハードウェア構成
	SYSTEM_INFO sysInfo = {};

	GetSystemInfo(&sysInfo);
}

#include <ShObjIdl.h> // IFileOpenDialog
#include <locale>
#include <codecvt>

std::string OpenImportFileDialog()
{
    //auto rs = DialogBox(m_hInstance, MAKEINTRESOURCE(IDD_DIALOG1), m_hWnd, FilePathDialogProc);
	IFileOpenDialog* pFileOpenDialog = nullptr;
	auto rs = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFileOpenDialog));
	if (FAILED(rs)) {
		pFileOpenDialog->Release();
		return "";
	}
	rs = pFileOpenDialog->Show(NULL);
	if (FAILED(rs)) {
		pFileOpenDialog->Release();
		return "";
	}
	IShellItem* pItem = nullptr;
	rs = pFileOpenDialog->GetResult(&pItem);
	if (FAILED(rs)) {
		pFileOpenDialog->Release();
		return "";
	}
	PWSTR pszFilePath = nullptr;
	rs = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
	pFileOpenDialog->Release();
	pItem->Release();
	//std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
	//std::string narrow_str = converter.to_bytes(pszFilePath);

	// 代替: WideCharToMultiByte を使って wstring → string 変換
	int len = WideCharToMultiByte(CP_UTF8, 0, pszFilePath, -1, nullptr, 0, nullptr, nullptr);
	std::string narrow_str;
	if (len > 0) {
	    narrow_str.resize(len - 1); // null終端を除く
	    WideCharToMultiByte(CP_UTF8, 0, pszFilePath, -1, &narrow_str[0], len, nullptr, nullptr);
	}
	return narrow_str;
}

std::string OpenExportFileDialog()
{
	IFileSaveDialog* pFileSaveDialog = nullptr;
	auto rs = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileSaveDialog));
	if (FAILED(rs)) {
		pFileSaveDialog->Release();
		return "";
	}
	pFileSaveDialog->SetFileName(L"scene.yml");
	pFileSaveDialog->SetDefaultExtension(L"yml");

	// COM_FILTERSPEC構造体の配列を定義
	const COMDLG_FILTERSPEC rgSpec[] =
	{
		{ L"YAMLファイル (*.yml)", L"*.yml" },
		{ L"テキストファイル (*.txt)", L"*.txt" },
		{ L"すべてのファイル (*.*)", L"*.*" },
	};

	// SetFileTypesでファイルの種類を設定
	rs = pFileSaveDialog->SetFileTypes(ARRAYSIZE(rgSpec), rgSpec);

	if (FAILED(rs)) {
		pFileSaveDialog->Release();
		return "";
	}
	
	// デフォルトで選択されるインデックスを設定（0から始まる）
	rs = pFileSaveDialog->SetFileTypeIndex(1); // CSVファイルがデフォルトで選択される
	
	rs = pFileSaveDialog->Show(NULL);
	if (FAILED(rs)) {
		pFileSaveDialog->Release();
		return "";
	}
	IShellItem* pItem = nullptr;
	rs = pFileSaveDialog->GetResult(&pItem);
	if (FAILED(rs)) {
		pFileSaveDialog->Release();
		return "";
	}
	PWSTR pszFilePath = nullptr;
	rs = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
	pFileSaveDialog->Release();
	pItem->Release();

	int len = WideCharToMultiByte(CP_UTF8, 0, pszFilePath, -1, nullptr, 0, nullptr, nullptr);
	std::string narrow_str;
	if (len > 0) {
		narrow_str.resize(len - 1); // null終端を除く
		WideCharToMultiByte(CP_UTF8, 0, pszFilePath, -1, &narrow_str[0], len, nullptr, nullptr);
	}
	return narrow_str;
}

INT_PTR MainEngine::FilePathDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	{
		static std::string* pInputValue = nullptr;
		switch (message)
		{
		case WM_INITDIALOG:
			pInputValue = reinterpret_cast<std::string*>(lParam);
			if (pInputValue && !pInputValue->empty())
			{
				SetDlgItemTextA(hDlg, IDC_RICHEDIT21, pInputValue->c_str()); // テキストボックスに値を設定
			}
			return (INT_PTR)TRUE;
		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK)
			{
				char buffer[1024] = { 0 };
				EndDialog(hDlg, LOWORD(wParam));
				std::list<Object*> objects;
				GetDlgItemTextA(hDlg, IDC_RICHEDIT21, buffer, sizeof(buffer));
				Editor::GetInstance()->ResetScene();
				//objects = CSVImporter::Import(buffer);
				if (objects.empty()) {
					MessageBoxA(hDlg, "読み込みに失敗しました。", "エラー", MB_OK | MB_ICONERROR);
				}
				else {
					Editor::GetInstance()->SetObjects(objects); // 読み込んだオブジェクトをエディターに設定
				}
				return (INT_PTR)TRUE;
			}
			else if (LOWORD(wParam) == IDCANCEL)
			{
				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}
			break;
		}
		return (INT_PTR)FALSE;
	}
}
