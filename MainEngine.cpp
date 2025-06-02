

#include "MainEngine.h"

#include <sstream>
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)

#include "FrameWork.h"

#include "Component_InputSystem.h"

constexpr auto WINDOW_CREATE_FAILED = -1;

MainEngine* MainEngine::m_pInstance = nullptr;


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	auto mainEngine = MainEngine::GetInstance();

	mainEngine->Initialize(hInstance, hPrevInstance, lpCmdLine, nCmdShow);

	auto param = mainEngine->SystemLoop();

	mainEngine->Finalize();

	return param;
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

			m_dwCurrentTime = timeGetTime(); // システム時刻を取得
			if ((m_dwCurrentTime - m_dwFPSLastTime) >= 1000) // 1秒ごとに実行
			{
#ifdef _DEBUG
				m_FramePerSecond = m_dwFrameCount;
#endif
				m_dwFPSLastTime = m_dwCurrentTime; // FPSを測定した時刻を保存
				m_dwFrameCount = 0; // カウントをクリア
			}
			if ((m_dwCurrentTime - m_dwExecLastTime) >= (1000 / 60)) // 1/60秒ごとに実行
			{
				m_dwExecLastTime = m_dwCurrentTime; // 処理した時刻を保存
#ifdef _DEBUG // デバッグ版の時だけFPSを表示する
				std::stringstream caption;
				caption << WINDOW_CAPTION << " FPS:" << m_FramePerSecond;
				SetWindowText(m_hWnd, caption.str().c_str());
#endif
				// ここがエンジン処理
				m_pEditor->Update();
				m_pEditor->Draw();

				m_dwFrameCount++; // 処理回数のカウントを加算
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

	// DWORD window_style = (WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME) ^ WS_MAXIMIZEBOX;
	DWORD windowStyle = WS_OVERLAPPEDWINDOW | WS_MAXIMIZEBOX | WS_THICKFRAME;
	//DWORD windowStyle = WS_OVERLAPPEDWINDOW;

	// 基本矩形座標
	RECT windowRect = { 0, 0, SCREEN_WIDTH_DEFAULT, SCREEN_HEIGHT_DEFAULT };

	// 指定したクライアント領域を確保するために新たな矩形座標を計算
	AdjustWindowRect(&windowRect, windowStyle, FALSE);

	// 新たなWindowの矩形座標から幅と高さを算出
	XMINT2 windowSize = { windowRect.right - windowRect.left, windowRect.bottom - windowRect.top };

	// プライマリモニターの画面解像度取得
	XMINT2 desktopSize = { GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };

	// デスクトップの真ん中にウィンドウが生成されるように座標を計算
	// ※ただし万が一、デスクトップよりウィンドウが大きい場合は左上に表示
	XMINT2 windowPos = {
		desktopSize.x - windowSize.x >= 0 ? desktopSize.x - windowSize.x : 0,
		desktopSize.x - windowSize.x >= 0 ? desktopSize.y - windowSize.y : 0
	};

	// ウィンドウの生成
	m_hWnd = CreateWindow(
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
	HMENU hMenu = CreateMenu();

	HMENU hEditorMenu = CreatePopupMenu();
	AppendMenu(hEditorMenu, MF_STRING, EngineMenu::Editor_NewWindow, "新規ウィンドウ");
	AppendMenu(hEditorMenu, MF_SEPARATOR, 0, NULL);
	AppendMenu(hEditorMenu, MF_STRING, EngineMenu::Editor_End, "終了");

	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hEditorMenu, "エディター");

	SetMenu(m_hWnd, hMenu);

	SetWindowPos(
		m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);


	SetWindowLongPtr(m_hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

//==========================================================================
// システム初期化
//==========================================================================


	HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);

	m_pRenderer = new Renderer(m_hWnd);

	m_pEditor = Editor::GetInstance();

	AllocConsole();
	freopen_s(&m_pFile, "CONOUT$", "w", stdout);

	//imGUI初期化
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	ImGui_ImplWin32_Init(m_hWnd);
	ImGui_ImplDX11_Init(
		m_pRenderer->GetDevice(),
		m_pRenderer->GetDeviceContext());

	// 指定のウィンドウハンドルのウィンドウを指定の方法で表示
	ShowWindow(m_hWnd, SW_MAXIMIZE);

	timeBeginPeriod(1); // 分解能を設定

	m_dwExecLastTime = m_dwFPSLastTime = timeGetTime(); // システム時刻をミリ秒単位で取得
	m_dwCurrentTime = m_dwFrameCount = 0;

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
		case EngineMenu::Editor_NewWindow: // 新規ウィンドウ
			MessageBox(hWnd, "ウィンドウを作りたかった", "CreateWindow", MB_OKCANCEL | MB_DEFBUTTON2);
			break;
		case EngineMenu::Editor_End: // 終了
			if (MessageBox(hWnd, "本当に終了してよろしいですか？", "確認", MB_OKCANCEL | MB_DEFBUTTON2) == IDOK) {
				DestroyWindow(hWnd); // 指定のウィンドウにWM_DESTROYメッセージを送る
			}
			return 0; // DefWindowProc関数にメッセージを流さず終了することによって何もなかったことにする
		default:
			break;
		}
		break;

	case WM_CLOSE:
		if (MessageBox(hWnd, "本当に終了してよろしいですか？", "確認", MB_OKCANCEL | MB_DEFBUTTON2) == IDOK) {
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

	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		Keyboard::GetInstance()->KeyboardProcessMessage(message, wParam, lParam);
		break;
	};

	// 通常メッセージ処理はこの関数に任せる
	return DefWindowProc(hWnd, message, wParam, lParam);

}
