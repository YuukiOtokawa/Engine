// RuntimeCompiledC++を参考にしたコンパイルシステム
// https://github.com/RuntimeCompiledCPlusPlus/RuntimeCompiledCPlusPlus.git

//
// Copyright (c) 2010-2011 Matthew Jack and Doug Binks
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

//
// Notes:
//   - We use a single intermediate directory for compiled .obj files, which means
//     we don't support compiling multiple files with the same name. Could fix this
//     with either mangling names to include paths,  or recreating folder structure
//
//

#include "Compiler.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include <sstream>
#include <vector>
#include <set>
#include "CompileFileSystem.h"

#include "assert.h"
#include <process.h>

#include "CompilerLogger.h"

using namespace std;
using namespace CompileFileSystem;

// Visual Studio バージョン情報構造体
struct VSVersionInfo
{
	std::string Path;
};

// コンパイル完了トークン
const std::string CompiletionToken("_COMPILETION_TOKEN_");

// Visual Studioインストールパスを取得
void GetPathsOfVisualStudioInstalls(std::vector<VSVersionInfo>* pVersions, CompilerLogger* pLogger);

// 出力を読み取り処理するスレッド関数
void ReadAndHandleOutputThread(LPVOID arg);

// コマンドプロセス構造体
struct CmdProcess
{
	CmdProcess();
	~CmdProcess();

	void InitializeProcess();
	void WriteInput(std::string& input);
	void CleanupProcessAndPipes();

	PROCESS_INFORMATION m_CmdProcessInfo;
	HANDLE m_CmdProcessOutputRead;
	HANDLE m_CmdProcessInputWrite;
	
	volatile bool m_isComplete;
	CompilerLogger* m_pLogger;
	bool m_isStoreCmdOutput; // コマンド出力を保存するか
	std::string m_CmdOutput; // コマンド出力
	CompileFileSystem::Path m_PathTempCLCommandFile; // 一時CLコマンドファイルのパス
		// コンパイラに渡すコマンドライン引数を記述したテキストファイル
};

// プラットフォームコンパイラ実装データクラス
class PlatformCompilerData
{
public:
	// プラットフォームコンパイラ実装データクラス
	PlatformCompilerData();
	~PlatformCompilerData();

	std::string m_VSPath;		// Visual Studioのパス
	bool m_isFoundVS;			// Visual Studioを検索するか
	CmdProcess m_CmdProcess;
	CompilerLogger* m_pLogger;
};

Compiler::Compiler()
	: m_pData(0)
	, m_isFastCompile(false)
{ }

Compiler::~Compiler()
{
	delete m_pData;
}

std::string Compiler::GetObjectFileExtension() const
{
	return ".obj";
}

bool Compiler::GetIsComplete() const
{
	bool isComplete = m_pData->m_CmdProcess.m_isComplete;
	if (isComplete && !m_isFastCompile)
	{
		m_pData->m_CmdProcess.CleanupProcessAndPipes();
	}
	return isComplete;
}

void Compiler::Initialize(CompilerLogger* pLogger)
{
	m_pData = new PlatformCompilerData;
	m_pData->m_pLogger = pLogger;
	m_pData->m_CmdProcess.m_pLogger = pLogger;
}

// 環境変数を展開するヘルパー関数
static inline std::string ExpandEnvVars(const std::string& string)
{
	// Windows APIのExpandEnvironmentStringsを使用して環境変数を展開
	
	// UTF-8からUTF-16に変換
	std::wstring temp = CompileFileSystem::_Win32Utf8ToUtf16(string);
	
	// 展開後の文字数を取得
	uint32_t numChars = ExpandEnvironmentStringsW(temp.c_str(), nullptr, 0);
	
	// 展開後の文字列
	std::wstring tempExpanded;
	tempExpanded.resize(numChars++); // documentation is a little unclear if null character is included
	
	// 環境変数を展開
	uint32_t numCharsExpanded = ExpandEnvironmentStringsW(temp.c_str(), &tempExpanded[0], numChars);
	
	// UTF-16からUTF-8に変換して返す
	return CompileFileSystem::_Win32Utf16ToUtf8(tempExpanded);
}

void Compiler::RunCompile(const std::vector<CompileFileSystem::Path>& filesToCompile,	// コンパイル対象となるファイルのパスのリスト
						  const CompilerOptions& compilerOptions,						// コンパイラの設定やオプションを指定するオブジェクト
						  const std::vector<CompileFileSystem::Path>& linkLibraryList,	// リンクするライブラリのパスのリスト
						  const CompileFileSystem::Path& moduleName						// 生成されるモジュールの名前またはパス
)
{
	// Visual Studioのパスを一度だけ検索
	if (!m_pData->m_isFoundVS) {
		m_pData->m_isFoundVS = false; // 一回だけ

		// Visual Studioのインストールパスを取得
		std::vector<VSVersionInfo> vsVersions;
		GetPathsOfVisualStudioInstalls(&vsVersions, m_pData->m_pLogger);

		// 最初に見つかったVisual Studioのパスを使用
		if (!vsVersions.empty())
		{
			m_pData->m_VSPath = vsVersions[0].Path;
		}
		else
		{
			m_pData->m_VSPath = "";
			if (m_pData->m_pLogger)
			{
				m_pData->m_pLogger->LogError("No Supported Compiler found.\n");
			}
		}
	}
	
	// Visual Studioのパスが見つからない場合、エラーログを出力して終了
	if (m_pData->m_VSPath.empty())
	{
		if (m_pData->m_pLogger)
		{
			m_pData->m_pLogger->LogError("No Supported Compiler found, cannot compile changes.\n");
		}
		m_pData->m_CmdProcess.m_isComplete = true;
		return;
	}
	m_pData->m_CmdProcess.m_isComplete = false;

	// コンパイラの場所
	std::string flags;
#ifdef _DEBUG
	flags = "/nologo /Z7 /FC /utf-8 /MTd /LDd ";
#else
	flags = "/nologo /O2 /FC /utf-8 /MT /LD "; // リリースでもデバッグ情報が必要
#endif // _DEBUG
	// コマンドの内容:
	// /nologo	コンパイラの著作権表示を非表示
	// /Z7		デバッグ情報をobjファイルに埋め込む
	// /O2		最適化レベル2（高速化）速度優先の最適化
	// /FC		エラーメッセージでソースファイルの完全なパスを表示
	// /utf-8	ソースファイルはUTF-8エンコードであると仮定
	// /MT		マルチスレッド静的ランタイムライブラリを使用(/MTd はデバッグ版)
	// /LD		DLL(ダイナミックリンクライブラリ)としてコンパイル(/LDd はデバッグ版)

	OptimizationLevel optimizationLevel = GetActualOptimizationLevel(compilerOptions.optimizationLevel);
	switch (optimizationLevel)
	{
	case OPTIMIZATIONLEVEC_DEFAULT: // デフォルト。だけどありえないはず
		assert(false);
	case OPTIMIZATIONLEVEL_DEBUG: // デバッグレベル
		flags += "/Od "; // 最適化を無効化(デバッグ用)
		break;
	case OPTIMIZATIONLEVEL_PERF: // パフォーマンスレベル
		flags += "/O2 "; // 最適化レベル2（高速化）速度優先の最適化

#if (_MSC_VER >= 1700) // Visual Studio 2012以降
		flags += "/d2Zi+ "; // 最適化されたコードのデバッグ情報を改善(Visual Studio 2012以降)
#endif
	default:;
	}

	if (NULL == m_pData->m_CmdProcess.m_CmdProcessInfo.hProcess)
	{
		m_pData->m_CmdProcess.InitializeProcess();

		// Visual Studioの環境変数を設定するバッチファイル
		std::string cmdSetParams = "\"" + m_pData->m_VSPath;
#ifndef _WIN64
		cmdSetParams += "Vcvarsall.bat\" x86\n";
#else
		cmdSetParams += "Vcvarsall.bat\" x86_amd64\n";
#endif // !_WIN64

		// 初期セットアップコマンドを送信
		m_pData->m_CmdProcess.WriteInput(cmdSetParams);
		// UTF-8コンソールロケールを設定
		std::string chcp65001 = "chcp 65001\n";
		m_pData->m_CmdProcess.WriteInput(chcp65001); 
	}

	flags += compilerOptions.compileOptions;
	flags += " ";

	// リンクオプションを作成
	// ライブラリディレクトリと追加のリンクオプションを処理
	std::string linkOptions;
	bool isHaveLinkOptions = (0 != compilerOptions.linkOptions.length());

	if (compilerOptions.libraryDirList.size() || isHaveLinkOptions)
	{
		linkOptions = " /link ";
		for (size_t i = 0; i < compilerOptions.libraryDirList.size(); i++)
		{
			linkOptions = " /link ";
			for (size_t i = 0; i < compilerOptions.libraryDirList.size(); i++)
			{
				linkOptions += " /LIBPATH:\"" + ExpandEnvVars(compilerOptions.libraryDirList[i].m_String) + "\"";
			}

			if (isHaveLinkOptions)
			{
				linkOptions += compilerOptions.linkOptions + " ";
			}
		}
	}

	// 中間ディレクトリの存在を確認し、必要に応じて作成する
	// ここでより多くのチェックとロバスト性を追加することができる
	// ロバスト性：予期しない状況やエラーでもシステムが適切に動作し続ける能力
	if (!compilerOptions.intermediatePath.Exists())
	{
		bool success = compilerOptions.intermediatePath.CreateDir();

		if (success && m_pData->m_pLogger)
		{
			m_pData->m_pLogger->LogInfo("Created intermediate folder \"%s\"\n", compilerOptions.intermediatePath.c_str());
		}
		else if (m_pData->m_pLogger)
		{
			m_pData->m_pLogger->LogError("Error creating intermediate folder \"%s\"\n", compilerOptions.intermediatePath.c_str());
		}
	}

	// インクルードファイルリストを作成
	std::string strIncludeFiles;
	for (size_t i = 0; i < compilerOptions.includeDirList.size(); i++)
	{
		strIncludeFiles += " /I \"" + ExpandEnvVars(compilerOptions.includeDirList[i].m_String) + "\"";
	}

	// マルチスレッドコンパイル使用時、同一ファイルを重複してコンパイル対象に指定するとエラーが発生する可能性があるため、
	// ここで入力の最終的なフィルタリングを行う。
	// 参照: http://msdn.microsoft.com/en-us/library/bb385193.aspx - "Source Files and Build Order"
	
	// 重複するパスをフィルタリングしながらコンパイルするファイルリストを作成
	std::string strFilesToCompile;
	std::set<std::string> filteredPaths;
	for (size_t i = 0; i < filesToCompile.size(); i++)
	{
		std::string strPath = ExpandEnvVars(filesToCompile[i].m_String);
		CompileFileSystem::ToLowerInPlace(strPath);

		std::set<std::string>::const_iterator it = filteredPaths.find(strPath);
		if (it == filteredPaths.end())
		{
			strFilesToCompile += " \"" + strPath + "\"";
			filteredPaths.insert(strPath);
		}
	}

	// リンクするライブラリリストを作成
	std::string strLinkLibraries;
	for (size_t i = 0; i < linkLibraryList.size(); i++)
	{
		strLinkLibraries += " \"" + ExpandEnvVars(linkLibraryList[i].m_String) + "\" ";
	}

	// 文字セットフラグを設定
#ifdef UNICODE
	const char* pCharTypeFlags = "/D UNICODE /D _UNICODE ";
#else
	const char* pCharTypeFlags = "";
#endif // UNICODE

	// コンパイラの場所を設定
	std::string compilerLocation = compilerOptions.compilerLocation.m_String;
	if (compilerLocation.size() == 0)
	{
#if defined __clang__
#ifndef _WIN64
		// 32ビットアーキテクチャを指定
		std::string arch = "-m32 ";
#else
		// 64ビットアーキテクチャを指定
		std::string arch = "-m64 ";
#endif // !_WIN64

		// フルパスとアーキテクチャを指定
		compilerLocation = "\"%VCINSTALLDIR%Tools\\Llvm\\bin\\clang-cl\" ";
		compilerLocation += arch;

#else
		// フルパスとアーキテクチャは不要、clコンパイラは既にVcvarsall.batで初期化されているため
		compilerLocation = "cl ";

#endif // defined __clang__
	}
	else if (compilerLocation.back() != ' ')
	{
		// 最後にスペースが必要
		compilerLocation += " ";
	}

	// コマンドラインオプションを作成
	// /MP はマルチプロセッサコンパイルを有効にする
	// 複数ファイルのコンパイル時にのみコンパイルが高速化される
	// リンクは高速化されない
	std::string clCommandOptions = flags + pCharTypeFlags
		+ " /MP /Fo\"" + compilerOptions.intermediatePath.m_String + "\\\\\" "
		+ "/D WIN32 /EHa /Fe" + moduleName.m_String;
	clCommandOptions += " " + strIncludeFiles + " " + strFilesToCompile + strLinkLibraries + linkOptions;
	if (m_pData->m_pLogger)
		m_pData->m_pLogger->LogInfo("%s", clCommandOptions.c_str()); // %sを使用して、コンパイル文字列内のトークンがフォーマットとして解釈されるのを防ぐ

	// コマンドラインの長さが制限を超える場合、コマンドファイルを使用してコンパイルオプションを渡す
	CompileFileSystem::Path pathTempCLCommandFile = compilerOptions.intermediatePath / "CLCommandFile.temp";
	// コマンドファイルを作成
	FILE* file = CompileFileSystem::fopen(pathTempCLCommandFile, "wb");

	// コマンドファイルを作成できた場合、コマンドファイルを使用してコンパイルオプションを渡す
	if (file)
	{
		// UTF-8 BOMをファイルの先頭に書き込む
		m_pData->m_CmdProcess.m_PathTempCLCommandFile = pathTempCLCommandFile;
		uint8_t utf_8_BOM[] = { 0xEF, 0xBB, 0xBF };
		fwrite(utf_8_BOM, 1, sizeof(utf_8_BOM), file);
		// コンパイルオプションを書き込む
		fwrite(clCommandOptions.c_str(), 1, clCommandOptions.size(), file);
		fclose(file);
		// コマンドファイルを使用してコンパイルオプションを渡す
		std::string cmdToSend = compilerLocation + " @" + pathTempCLCommandFile.GetOSShortForm().m_String;
		cmdToSend += "\necho ";
		cmdToSend += CompiletionToken + "\n";
		// コマンドを送信
		m_pData->m_CmdProcess.WriteInput(cmdToSend);
	}
	else
	{
		// %sを使用して、コンパイル文字列内のトークンがフォーマットとして解釈されるのを防ぐ
		if (m_pData->m_pLogger)
			m_pData->m_pLogger->LogInfo("Could not create CL Command File %s\n  - Falling back to command line options\n", pathTempCLCommandFile.c_str());

		m_pData->m_CmdProcess.m_PathTempCLCommandFile.m_String.clear(); // コマンドファイルを作成できない - コマンドラインにフォールバック
		// コマンドを送信
		std::string cmdToSend = compilerLocation + clCommandOptions;
		cmdToSend += "\necho ";
		cmdToSend += CompiletionToken + "\n";
		m_pData->m_CmdProcess.WriteInput(cmdToSend);
	}
}

/// @brief レジストリキーに関する情報を格納する構造体です。
struct VSKey
{
	const char* keyName;
	const char* pathToAdd;
	HKEY		key;
};

/// @brief Visual Studio のバージョン情報を格納する構造体です。
struct VSVersionDiscoveryInfo
{
	const char* versionName;
	const char* versionNextName;	// vswhereクエリには範囲が必要なため、「次の」バージョン名が必要 - 配列で+1を使用するのではなく格納する（存在しないか正しくない可能性があるため）
	int			versionKey;			// VSKey値の配列へのインデックス、-1は検索しない
	bool		tryVSWhere;			// VSWhereを使用できるかどうか
};

/// @brief 検索可能な Visual Studio のインストールパスを検出し、結果を pVersions に格納します。検出にはレジストリ検索と vswhere ユーティリティの利用が含まれます。検出過程の警告や情報は pLogger に記録されます。
/// @param pVersions 検出された Visual Studio インストール情報（パスなど）を格納する VSVersionInfo の std::vector へのポインタ。
/// @param pLogger 検出過程の警告や情報を記録するための CompilerLogger へのポインタ。NULL の場合はログ出力されません。
void GetPathsOfVisualStudioInstalls(std::vector<VSVersionInfo>* pVersions, CompilerLogger* pLogger)
{
	//e.g.: HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\VisualStudio\<version>\Setup\VS\<edition>
	// to view 32bit keys on Windows use start->run and enter: %systemroot%\syswow64\regedit
	// as for 32bit keys need to run 32bit regedit.

	// Visual Studioのインストールパスを検索するためのレジストリキー
	VSKey VS_Keys[] =
	{
		{"SOFTWARE\\Microsoft\\VisualStudio\\SxS\\VC7", "", NULL},
		{"SOFTWARE\\Microsoft\\VisualStudio\\SxS\\VS7", "VC\\Auxiliary\\Build\\", NULL}
	};

	// VS_Keys配列の要素数
	int NUM_VS_KEYS = sizeof(VS_Keys) / sizeof(VSKey);

	// サポートされているVisual Studioのバージョン
	VSVersionDiscoveryInfo VS_DISCOVERY_INFO[] =
	{
		{"17.0", "18.0", 1, true},	// Visual Studio 2022
		{"16.0", "17.0", 1, true},	// Visual Studio 2019
		{"15.0", "16.0", 1, true},	// Visual Studio 2017
		{"14.0", "15.0", 0, false}, // Visual Studio 2015
		{"12.0", "14.0", 0, false}, // Visual Studio 2013
		{"11.0", "12.0", 0, false}, // Visual Studio 2012
		{"10.0", "11.0", 0, false}, // Visual Studio 2010
		{"9.0",  "10.0", 0, false}, // Visual Studio 2008
		{"8.0",  "9.0",  0, false}  // Visual Studio 2005
	};

	int NUM_NAMES_TO_CHECK = sizeof(VS_DISCOVERY_INFO) / sizeof(VSVersionDiscoveryInfo);
	// 現在のバージョンから互換性のあるコンパイラを逆順に検索し始める
	int startVersion = 0;

#if !defined __clang__
	// Microsoft Visual C++ コンパイラーのバージョンをチェックし、使用したコンパイラーに基づいて検索を開始する
	// clangではなくMSC_VERをチェックする - clangはデフォルトで1800を報告するため
	const UINT MSCVERSION = _MSC_VER;
	bool isMSCVersionFound = true; // 一つの偽ケースのみなのでデフォルトでtrue
	switch (MSCVERSION)
	{
	case 1931: // VS 2022
	case 1930:
		startVersion = 0;
		break;
	case 1929: // VS 2019
	case 1928:
	case 1927:
	case 1926:
	case 1925:
	case 1924:
	case 1923:
	case 1922:
	case 1921:
		case 1920:
		startVersion = 1;
		break;
	case 1916: // VS 2017
	case 1915:
	case 1914:
	case 1913:
	case 1912:
	case 1911:
	case 1910:
		startVersion = 2;
		break;
	case 1900:	// VS 2015
		startVersion = 3;
		break;
	case 1800:	// VS 2013
		startVersion = 4;
		break;
	case 1700:	// VS 2012
		startVersion = 5;
		break;
	case 1600:	// VS 2010
		startVersion = 6;
		break;
	case 1500:	// VS 2008
		startVersion = 7;
		break;
	case 1400:	// VS 2005
		startVersion = 8;
		break;
	default:
		isMSCVersionFound = false;
		if (pLogger)
		{
			pLogger->LogWarning("WARNING: VS Compiler with _MSC_VER %d potentially not supported. Defaulting to version %s.\n", MSCVERSION, VS_DISCOVERY_INFO[startVersion].versionName);
		}
		break;
	}
#endif // !defined __clang__

	char value[MAX_PATH];
	DWORD size = MAX_PATH;

	// レジストリキーを開く
	for (int i = 0; i < NUM_VS_KEYS; i++)
	{
		LONG retKeyVal = RegOpenKeyExA(
			HKEY_LOCAL_MACHINE,						//__in			HKEY hKey,
			VS_Keys[i].keyName,				//__in_opt		LPCSTR lpSubKey,
			0,								//__reserved	DWORD ulOptions,
			KEY_READ | KEY_WOW64_32KEY,	//__in			REGSAM samDesired,
			&VS_Keys[i].key					//__out_opt		PHKEY phkResult
		);
	}

	int loopCount = 1;
	if (startVersion != NUM_NAMES_TO_CHECK - 1)
	{
		// 最新バージョンから始めていない場合、検索を最初から再度実行する必要があるかもしれない
		loopCount = 2;
	}

	// 指定されたバージョンから始めて、すべてのバージョンを逆順にチェックする
	for (int loop = 0; loop < loopCount; loop++)
	{
		for (int i = startVersion; i >= 0; i--)
		{
			VSVersionDiscoveryInfo vsInfo = VS_DISCOVERY_INFO[i];
			VSKey                  vsKey = VS_Keys[vsInfo.versionKey];

			if (vsInfo.tryVSWhere)
			{
				// vswhereを使用してVisual Studioのインストールパスを取得する
				CmdProcess cmdProc;
				cmdProc.m_pLogger = pLogger;
				cmdProc.InitializeProcess();
				cmdProc.m_isStoreCmdOutput = true;
				cmdProc.m_CmdOutput = "";

				std::string maxVersion;

				if (!isMSCVersionFound && i == startVersion)
				{
					maxVersion = "";
				}
				else
				{
					maxVersion = vsInfo.versionNextName;
				}

				// vswhereクエリを作成
				std::string vsWhereQuery =
					"\"%ProgramFiles(x86)%\\Microsoft Visual Studio\\Installer\\vswhere\""
					" -version [" + std::string(vsInfo.versionName) + "," + maxVersion + ")"
					" -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64"
					" -property installationPath"
					"\nexit\n";

				// クエリを送信
				cmdProc.WriteInput(vsWhereQuery);
				WaitForSingleObject(cmdProc.m_CmdProcessInfo.hProcess, 2000); // 最大2秒

				size_t start = cmdProc.m_CmdOutput.find_first_not_of("\r\n", 0);
				if (start != std::string::npos)
				{
					size_t end = cmdProc.m_CmdOutput.find_first_of("\r\n", start);
					if (end == std::string::npos)
					{
						end = cmdProc.m_CmdOutput.length();
					}
					
					CompileFileSystem::Path path = cmdProc.m_CmdOutput.substr(start, end - start);
					if (path.m_String.length() && path.Exists())
					{
						VSVersionInfo vInfo;
						vInfo.Path = path.m_String + "\\" + vsKey.pathToAdd;
						pVersions->push_back(vInfo);
						continue;
					}
				}
			}

			// レジストリを使用してVisual Studioのインストールパスを取得する
			LONG retVal = RegQueryValueExA(
				vsKey.key,
				vsInfo.versionName,
				NULL,
				NULL,
				(LPBYTE)value,
				&size
			);
			if (retVal == ERROR_SUCCESS)
			{
				// 値が見つかった - パスを保存する
				VSVersionInfo vInfo;
				vInfo.Path = value;
				vInfo.Path += vsKey.pathToAdd;
				pVersions->push_back(vInfo);
			}
		}

		startVersion = NUM_NAMES_TO_CHECK - 1; // 2回目のループでは、最も古いバージョンから始める
	}

	for (int i = 0; i < NUM_VS_KEYS; i++)
	{
		RegCloseKey(VS_Keys[i].key);
	}
	return;
}

// 出力を読み取り処理するスレッド関数
void ReadAndHandleOutputThread(LPVOID arg)
{
	CmdProcess* pCmdProc = (CmdProcess*)arg;

	CHAR lpBuffer[1024];
	DWORD dwRead;
	bool isReadActive = true;

	// 出力を読み取り続ける
	while (isReadActive)
	{// 出力を読み取る
		if (!ReadFile(pCmdProc->m_CmdProcessOutputRead, lpBuffer, sizeof(lpBuffer) - 1, &dwRead, NULL) || !dwRead)
		{
			// 読み取りが何らかの原因で終了
			isReadActive = false;
			if (GetLastError() != ERROR_BROKEN_PIPE)
			{
				if (pCmdProc->m_pLogger)
					pCmdProc->m_pLogger->LogError("[RuntimeCompiler] Compile Outputへのリダイレクトが読み取りで失敗しました\n");
				pCmdProc->m_isComplete = true;
			}
		}
		else
		{
			// 出力を処理する
			lpBuffer[dwRead] = 0;
			std::string buffer(lpBuffer);

			// コンパイル完了トークンをチェック
			size_t found = buffer.find(CompiletionToken);
			if (found != std::string::npos)
			{// 完了トークンが見つかった
				// std::string::nposは見つからなかったことを示す特殊な値
				buffer = buffer.substr(0, found);
				if (!pCmdProc->m_isStoreCmdOutput&&pCmdProc->m_pLogger)
					pCmdProc->m_pLogger->LogInfo("[RuntimeCompiler] 完了\n");

				pCmdProc->m_isComplete = true;
			}

			// 出力をログに記録する
			if (isReadActive || buffer.length())
			{
				// 出力を保存するか、ログに記録する
				if (pCmdProc->m_isStoreCmdOutput)
					pCmdProc->m_CmdOutput += buffer;
				else
				{
					size_t errorFound = buffer.find(" : error ");
					size_t fatalErrorFound = buffer.find(" : fatal error ");

					if ((errorFound != std::string::npos) || (fatalErrorFound != std::string::npos))
					{
						if (pCmdProc->m_pLogger) pCmdProc->m_pLogger->LogError("%s", buffer.c_str());
					}
					else
					{
						if (pCmdProc->m_pLogger) pCmdProc->m_pLogger->LogInfo("%s", buffer.c_str());
					}
				}
			}
		}
	}
}

PlatformCompilerData::PlatformCompilerData()
	: m_isFoundVS(true)
	, m_pLogger(NULL)
{
}

PlatformCompilerData::~PlatformCompilerData()
{
}

CmdProcess::CmdProcess()
	: m_CmdProcessOutputRead(NULL)
	, m_CmdProcessInputWrite(NULL)
	, m_isComplete(false)
	, m_pLogger(NULL)
	, m_isStoreCmdOutput(false)
{
	ZeroMemory(&m_CmdProcessInfo, sizeof(m_CmdProcessInfo));
}

void CmdProcess::InitializeProcess()
{
	// コンパイルプロセスの初期化
	STARTUPINFOW si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);

	// コマンドプロセスの作成
	// コマンドプロセスを静かに起動し、終了しないようにする
	const wchar_t* pCommandLine = L"cmd /q /K @PROMPT $";

	// セキュリティ属性構造体を設定する。
	// 子プロセスがハンドルを継承できるようにするために必要
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);	// 構造体のサイズ
	sa.lpSecurityDescriptor = NULL;				// デフォルトのセキュリティ記述子
	sa.bInheritHandle = TRUE;					// 子プロセスがハンドルを継承できるようにする

	// 出力パイプの作成
	si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE; // ウィンドウを表示しない
	HANDLE hOutputReadTmp = NULL, hOutputWrite = NULL, hErrorWrite = NULL;
	if (!CreatePipe(&hOutputReadTmp, &hOutputWrite, &sa, 20 * 1024)) {
		if (m_pLogger) m_pLogger->LogError("[RuntimeCompiler] Failed to create output redirection pipe \n");
		goto ERROR_EXIT;
	}
	// 出力パイプの書き込みハンドルを標準出力に設定
	si.hStdOutput = hOutputWrite;

	// エラーパイプの作成
	if (!DuplicateHandle(GetCurrentProcess(), hOutputWrite,
		GetCurrentProcess(), &hErrorWrite, 0,
		TRUE, DUPLICATE_SAME_ACCESS))
	{
		if (m_pLogger) m_pLogger->LogError("[RuntimeCompiler] Failed to duplicate error output redirection pipe\n");
		goto ERROR_EXIT;
	}
	// エラーパイプの書き込みハンドルを標準エラー出力に設定
	si.hStdError = hErrorWrite;

	// 入力パイプの作成
	if (si.hStdOutput) {
		if (!DuplicateHandle(GetCurrentProcess(),	// 出力パイプの読み取りハンドルを複製
			hOutputReadTmp,								// 複製するハンドル
			GetCurrentProcess(),					// 複製先のプロセスハンドル
			&m_CmdProcessOutputRead,					// 複製されたハンドルの受け取り先
			0, FALSE,
			DUPLICATE_SAME_ACCESS)) {						// 同じアクセス権を持つ複製
			if (m_pLogger) m_pLogger->LogError("[RuntimeCompiler] Failed to duplicate output read pipe\n");
			goto ERROR_EXIT;
		}
		CloseHandle(hOutputReadTmp);
		hOutputReadTmp = NULL;
	}

	HANDLE hInputRead, hInputWriteTmp;
	
	// 入力パイプの作成
	if (!CreatePipe(&hInputRead, &hInputWriteTmp, &sa, 4096)) {
		if (m_pLogger) m_pLogger->LogError("[RuntimeCompiler] Failed to create input pipes\n");
		goto ERROR_EXIT;
	}
	si.hStdInput = hInputRead;

	// 入力パイプの書き込みハンドルを保存
	if (si.hStdOutput) {
		if (!DuplicateHandle(GetCurrentProcess(), hInputWriteTmp,
			GetCurrentProcess(),
			&m_CmdProcessInputWrite,
			0, FALSE,
			DUPLICATE_SAME_ACCESS)) {
			if (m_pLogger) m_pLogger->LogError("[RuntimeCompiler] Failed to duplicate input write pipe\n");
			goto ERROR_EXIT;
		}
	}

	wchar_t pCmdLineNonConst[1024];
	wcscpy_s(pCmdLineNonConst, pCommandLine);
	CreateProcessW(
		NULL,
		pCmdLineNonConst,
		NULL,
		NULL,
		TRUE,
		0,
		NULL,
		NULL,
		&si,
		&m_CmdProcessInfo
	);

	_beginthread(ReadAndHandleOutputThread, 0, this);

ERROR_EXIT:
	if (hOutputReadTmp) CloseHandle(hOutputReadTmp);
	if (hOutputWrite) CloseHandle(hOutputWrite);
	if (hErrorWrite) CloseHandle(hErrorWrite);
}

void CmdProcess::WriteInput(std::string& input) {
	DWORD nBytesWritten;
	DWORD length = (DWORD)input.length();
	WriteFile(m_CmdProcessInputWrite, input.c_str(), length, &nBytesWritten, NULL);
}

void CmdProcess::CleanupProcessAndPipes() {
	if (!m_PathTempCLCommandFile.m_String.empty() && m_PathTempCLCommandFile.Exists()) {
		m_PathTempCLCommandFile.Remove();
		m_PathTempCLCommandFile.m_String.clear();
	}

	if (m_CmdProcessInfo.hProcess) {
		TerminateProcess(m_CmdProcessInfo.hProcess, 0);
		TerminateThread(m_CmdProcessInfo.hThread, 0);
		CloseHandle(m_CmdProcessInfo.hThread);
		ZeroMemory(&m_CmdProcessInfo, sizeof(m_CmdProcessInfo));
		CloseHandle(m_CmdProcessInputWrite);
		m_CmdProcessInputWrite = 0;
		CloseHandle(m_CmdProcessOutputRead);
		m_CmdProcessOutputRead = 0;
	}
}

CmdProcess::~CmdProcess() {
	CleanupProcessAndPipes();
}

