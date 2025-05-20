#pragma once

// ========================================================
//
// エンジンシステム[MainEngine.h]
// 
//									Date:20250514
//									Author:Yuuki Otokawa
// ========================================================

#include <Windows.h>

#include <fstream>

#include "Renderer.h"
#include "Editor.h"

class MainEngine
{
private:
	enum EngineMenu {
		Editor_NewWindow = 1,
		Editor_End,
	};

	Renderer* m_pRenderer;
	Editor* m_pEditor;

	FILE* m_pFile;


	static DWORD g_CountFPS;


	//FPS制限用時間計測変数
	DWORD m_dwExecLastTime = 0;
	DWORD m_dwFPSLastTime = 0;
	DWORD m_dwCurrentTime = 0;
	DWORD m_dwFrameCount = 0;

	int m_FramePerSecond;


	MSG m_Message = {};

	//メインウィンドウハンドル
	HWND m_hWnd = NULL;


	HINSTANCE m_hInstance = NULL;


	//コンストラクタ
	MainEngine() = default;
	MainEngine(const MainEngine&) = delete;
	MainEngine& operator=(const MainEngine&) = delete;

	//デストラクタ
	~MainEngine() {
		
	}

	//インスタンスポインタ
	static MainEngine* m_pInstance;

public:
	static MainEngine* GetInstance() {
		if (m_pInstance == nullptr) {
			m_pInstance = new MainEngine();
		}
		return m_pInstance;
	}

	int SystemLoop();

	int Initialize(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
	void Finalize();

	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	LRESULT Proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	HWND GetWindow() { return m_hWnd; }
	HINSTANCE GetInstanceHandle() { return m_hInstance; }
	Renderer* GetRenderer() { return m_pRenderer; }

};

