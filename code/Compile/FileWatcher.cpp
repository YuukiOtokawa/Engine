// RuntimeCompiledC++を参考にしたコンパイルシステム
// https://github.com/RuntimeCompiledCPlusPlus/RuntimeCompiledCPlusPlus.git

/*
	Main header for the FileWatcher class. Declares all implementation
	classes to reduce compilation overhead.

	@author James Wynn
	@date 4/15/2009

	Copyright (c) 2009 James Wynn (james@jameswynn.com)

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in
	all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
	THE SOFTWARE.
*/


#include "FileWatcher.h"

#include <Windows.h>

namespace FW
{
	/// @brief ディレクトリ監視に関する情報を保持する構造体です。
	struct WatchStruct
	{
		OVERLAPPED m_Overlapped;				// 非同期I/O操作用のオーバーラップ構造体。ReadDirectoryChangesWの完了通知などに使用。
		HANDLE m_DirHandle;						// 監視対象ディレクトリのハンドル。
		BYTE m_Buffer[32 * 1024];				// 変更通知情報を受け取るバッファ。ReadDirectoryChangesWで利用。
		LPARAM lParam;							// ユーザー定義の追加情報を格納できるパラメータ（拡張用）。
		DWORD m_NotifyFilter;					// 監視するイベント種別（作成・削除・変更など）。ReadDirectoryChangesWの引数に渡す。
		bool m_StopNow;							// 監視停止フラグ。監視ループの終了判定などに利用。
		FileWatcher* m_pFileWatcher;			// この監視を管理するFileWatcherインスタンスへのポインタ。
		FileWatchListener* m_pFileWatchListener;// 監視イベントを受け取るリスナーへのポインタ。
		char* m_DirName;						// 監視対象ディレクトリのパス（文字列）。
		WatchID m_Watchid;						// 監視ID。監視対象を一意に識別するためのID。
		bool m_IsRecursive;						// サブディレクトリも再帰的に監視するかどうかのフラグ。
	};


#pragma region Internal Functions

	// 前方宣言
	bool RefreshWatch(WatchStruct* pWatch, bool _clear = false);

	// イベントを展開してコールバックに渡す
	
	/// @brief ファイルシステムの変更通知を処理するコールバック関数です。
	/// @param dwErrorCode 完了したI/O操作のエラーコード。0以外の場合はエラーを示します。
	/// @param dwNumberOfBytesTransfered 転送されたバイト数。0の場合はデータがないことを示します。
	/// @param lpOverlapped オーバーラップ構造体へのポインタ。監視情報を含みます。
	void CALLBACK WatchCallback(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped)
	{
		if (dwNumberOfBytesTransfered == 0) // No data, just return
			return;

		if (dwErrorCode != 0 || dwNumberOfBytesTransfered == 0)
		{// Error occurred, just return
			return;
		}

		// sz : ヌル終端文字列バッファ
		// ファイル名を格納
		char szFile[MAX_PATH];

		// ファイルシステム変更通知情報
		PFILE_NOTIFY_INFORMATION pNotify;

		// オーバーラップ構造体からWatchStructを取得
		WatchStruct* pWatch = (WatchStruct*)lpOverlapped;
		// オフセット初期化
		size_t offset = 0;

		do
		{
			// 次の通知情報を取得
			pNotify = (PFILE_NOTIFY_INFORMATION)&pWatch->m_Buffer[offset];
			// 次のエントリへのオフセットを更新
			offset += pNotify->NextEntryOffset;

			// ファイル名をマルチバイト文字列に変換
			/// @note WideCharToMultiByte関数を使用して、Unicode（UTF-16）からANSIコードページへの変換を行います。
			int count = WideCharToMultiByte(CP_ACP, 0, pNotify->FileName, pNotify->FileNameLength / sizeof(WCHAR), szFile, MAX_PATH - 1, NULL, NULL);
			// ファイル名の最終文字を設定
			szFile[count] = TEXT('\0');

			// 受け取ったファイル名とアクション(ファイル作成・削除など)を渡して、実際の処理を行う
			pWatch->m_pFileWatcher->HandleAction(pWatch, szFile, pNotify->Action);
		} while (pNotify->NextEntryOffset != 0);

		// 次の変更通知を受け取れるようにする
		RefreshWatch(pWatch);
	}

	/// @brief ディレクトリ監視の状態を更新します。
	/// @param pWatch 監視対象ディレクトリの情報を保持する WatchStruct 構造体へのポインタ。
	/// @param _clear コールバックをクリアするかどうかを指定するフラグ。true の場合はコールバックを使用しません。
	/// @return 監視の更新に成功した場合は true、失敗した場合は false を返します。
	bool RefreshWatch(WatchStruct* pWatch, bool _clear)
	{
		
		return ReadDirectoryChangesW(
			pWatch->m_DirHandle,		// 監視対象ディレクトリのハンドル
			pWatch->m_Buffer,			// 変更情報を受け取るバッファ
			sizeof(pWatch->m_Buffer),	// バッファサイズ
			TRUE,						// サブディレクトリの監視
			pWatch->m_NotifyFilter,		// 監視する変更の種類
			NULL, &pWatch->m_Overlapped,// 非同期I/O用構造体
			_clear ? 0 : WatchCallback	// 完了コールバック
		) != 0;
	}

	/// @brief WatchStruct オブジェクトと関連リソースを解放します。
	/// @param pWatch 解放する WatchStruct 構造体へのポインタ。
	void DestroyWatch(WatchStruct* pWatch)
	{
		if (pWatch)
		{
			// 監視終了フラグを立てる
			pWatch->m_StopNow = TRUE;

			// 監視ディレクトリハンドルの未完了の監視をキャンセル
			CancelIo(pWatch->m_DirHandle);

			// コールバックをクリア、監視を解除
			RefreshWatch(pWatch, true);

			// I/Oが完了するまで待機
			if (!HasOverlappedIoCompleted(&pWatch->m_Overlapped))
			{
				SleepEx(5, TRUE);
			}

			// イベントハンドルとディレクトリハンドルを閉じる
			CloseHandle(pWatch->m_Overlapped.hEvent);
			CloseHandle(pWatch->m_DirHandle);

			// メモリ開放
			delete[] pWatch->m_DirName;
			HeapFree(GetProcessHeap(), 0, pWatch);
		}
	}

	WatchStruct* CreateWatch(const char* szDirectory, bool recursive, DWORD notifyFilter)
	{
		WatchStruct* pWatch;
		size_t ptrSize = sizeof(*pWatch);
		// メモリを確保
		pWatch = static_cast<WatchStruct*>(HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, ptrSize));

		std::wstring convertedString;
		// ワイド文字列での文字数を取得
		int requiredSize = MultiByteToWideChar(CP_UTF8, 0, szDirectory, -1, 0, 0);

		if (requiredSize > 0)
		{
			convertedString.resize(requiredSize);
			MultiByteToWideChar(CP_UTF8, 0, szDirectory, -1, &convertedString[0], requiredSize);
			convertedString.pop_back(); // NULL終端文字を削除
		}

		pWatch->m_DirHandle = CreateFileW(convertedString.c_str(), FILE_LIST_DIRECTORY,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL,
			OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL);

		if (pWatch->m_DirHandle != INVALID_HANDLE_VALUE)
		{
			pWatch->m_Overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
			pWatch->m_NotifyFilter = notifyFilter;
			pWatch->m_IsRecursive = recursive;

			if (RefreshWatch(pWatch))
				return pWatch;
			else
			{
				CloseHandle(pWatch->m_Overlapped.hEvent);
				CloseHandle(pWatch->m_DirHandle);
			}
		}

		HeapFree(GetProcessHeap(), 0, pWatch);
		return NULL;
	}

#pragma endregion

	FileWatcher::FileWatcher() : m_LastWatchID(0)
	{
	}

	FileWatcher::~FileWatcher()
	{
		for (auto it : m_Watches)
		{
			DestroyWatch(it.second);
		}
		m_Watches.clear();
	}

	WatchID FileWatcher::AddWatch(const String& directory, FileWatchListener* watcher, bool recursive)
	{
		WatchStruct* watch = CreateWatch(directory.c_str(), recursive,
			FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_FILE_NAME);

		WatchID watchID = (unsigned long)fw.add(directory.);
	}
}