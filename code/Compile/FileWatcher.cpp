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

	/// @brief 指定したディレクトリの監視を行うWatchStructを作成します。
	/// @param szDirectory 監視対象ディレクトリのパス（UTF-8エンコードされた文字列）。
	/// @param recursive サブディレクトリも再帰的に監視するかどうかを指定します（trueで再帰的に監視）。
	/// @param notifyFilter 監視する変更イベントの種類を指定するDWORD値。
	/// @return 作成されたWatchStructへのポインタ。作成に失敗した場合はNULLを返します。
	WatchStruct* CreateWatch(const char* szDirectory, bool recursive, DWORD notifyFilter)
	{
		WatchStruct* pWatch;
		size_t ptrSize = sizeof(*pWatch);
		// メモリを確保
		pWatch = static_cast<WatchStruct*>(HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, ptrSize));

		std::wstring convertedString;
		// 監視対象ディレクトリのワイド文字列での文字数を取得
		int requiredSize = MultiByteToWideChar(CP_UTF8, 0, szDirectory, -1, 0, 0);

		// 監視対象ディレクトリをワイド文字列に変換
		if (requiredSize > 0)
		{
			convertedString.resize(requiredSize);
			MultiByteToWideChar(CP_UTF8, 0, szDirectory, -1, &convertedString[0], requiredSize);
			convertedString.pop_back(); // NULL終端文字を削除
		}

		// 監視対象ディレクトリののハンドルを取得
		pWatch->m_DirHandle = CreateFileW(convertedString.c_str(), FILE_LIST_DIRECTORY,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL,
			OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL);

		// ハンドルが有効なら初期化を続行
		if (pWatch->m_DirHandle != INVALID_HANDLE_VALUE)
		{
			// オーバーラップ構造体のイベントハンドルを作成
			pWatch->m_Overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
			// 監視停止フラグを初期化
			pWatch->m_NotifyFilter = notifyFilter;
			pWatch->m_IsRecursive = recursive;

			// 監視状態を更新
			if (RefreshWatch(pWatch))
			{// 成功したらWatchStructを返す
				return pWatch;
			}
			else
			{// 失敗したらリソースを解放
				CloseHandle(pWatch->m_Overlapped.hEvent);
				CloseHandle(pWatch->m_DirHandle);
			}
		}

		// 失敗した場合はメモリを解放してNULLを返す
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

	/// @brief 指定したディレクトリを監視対象として追加し、監視IDを返します。
	/// @param directory 監視するディレクトリのパスを表す文字列。
	/// @param watcher ファイル変更イベントを受け取るリスナーへのポインタ。
	/// @param recursive サブディレクトリも再帰的に監視するかどうかを指定するブール値。
	/// @return 新しく追加された監視対象に割り当てられたWatchID。
	WatchID FileWatcher::AddWatch(const String& directory, FileWatchListener* watcher, bool recursive)
	{
		// 監視対象ディレクトリのWatchStructを作成
		WatchStruct* watch = CreateWatch(directory.c_str(), recursive,
			FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_FILE_NAME);

		// 監視対象ディレクトリを追加し、監視IDを取得
		WatchID watchID = (unsigned long)m_pAltWatcher->Add(directory.m_String);

		// 監視対象ディレクトリのWatchStructが有効なら初期化
		if (watch)
		{// 監視ID、FileWatcherインスタンス、リスナー、ディレクトリ名を設定
			watch->m_Watchid = watchID;
			watch->m_pFileWatcher = this;
			watch->m_pFileWatchListener = watcher;
			watch->m_DirName = new char[directory.m_String.length() + 1];
			strcpy(watch->m_DirName, directory.c_str());
		}

		// 監視対象ディレクトリをマップに追加
		m_Watches.insert(std::make_pair(watchID, watch));

		return watchID;
	}

	/// @brief 指定されたディレクトリのウォッチを削除します。
	/// @param directory ウォッチを削除する対象のディレクトリ名。
	void FileWatcher::RemoveWatch(const String& directory)
	{
		// 監視対象ディレクトリを検索し、見つかったら削除
		for (auto it = m_Watches.begin(); it != m_Watches.end(); ++it)
		{
			if (directory == it->second->m_DirName)
			{
				RemoveWatch(it->first);
				return;
			}
		}
	}

	/// @brief 指定されたウォッチIDに対応するファイル監視を削除します。
	/// @param watchid 削除するファイル監視のID。
	void FileWatcher::RemoveWatch(WatchID watchid)
	{
		// 監視IDに対応するWatchStructを検索し、見つかったら削除
		auto it = m_Watches.find(watchid);
		if (it == m_Watches.end())
			return;
		WatchStruct* watch = it->second;
		m_pAltWatcher->Remove(watch->m_Watchid);
		m_Watches.erase(it);
		DestroyWatch(watch);
	}

	/// @brief ファイルシステムの変更イベントを監視し、検出されたイベントに応じて適切なアクションを実行します。
	void FileWatcher::Update()
	{
		// メッセージキューをチェックし、すべての入力メッセージを処理
		MsgWaitForMultipleObjectsEx(0, NULL, 0, QS_ALLINPUT, MWMO_ALERTABLE);

		// 監視イベントを格納するベクター
		static std::vector<FileWatcher_Alt::FW_Event> events;
		events.clear();

		// 監視イベントを取得
		m_pAltWatcher->Watch(events);

		// 取得したイベントを処理
		for (size_t c = 0; c != events.size(); c++) 
		{
			// 監視IDに対応するWatchStructを検索
			WatchMap::iterator it = m_Watches.find(events[c].id);
			if (it == m_Watches.end())
				continue;
			std::string oldName;
			DWORD fni;
			std::string newName = m_pAltWatcher->GetEventFileName(it->second->m_DirName, events[c].id, events[c].type, fni, oldName);


			switch (events[c].type)
			{
			case FileWatcher_Alt::CHANGE_SIZE:
			case FileWatcher_Alt::CHANGE_LAST_WRITE:
			case FileWatcher_Alt::CHANGE_CREATION:
				if (!newName.empty())
				{
					HandleAction(it->second, newName.c_str(), fni);
				}

				break;

			case FileWatcher_Alt::CHANGE_FILE_NAME:
				if (!newName.empty())
				{
					HandleAction(it->second, newName.c_str(), FILE_ACTION_RENAMED_OLD_NAME);
					HandleAction(it->second, newName.c_str(), FILE_ACTION_RENAMED_NEW_NAME);
				}
				else
				{
					HandleAction(it->second, oldName.c_str(), fni);
				}
				break;

			default:
				break;
			}
		}
	}

	/// @brief ファイルシステムのアクションを処理し、リスナーに通知します。
	/// @param watch 監視対象ディレクトリの情報を保持するWatchStructへのポインタ。
	/// @param fileName アクションが発生したファイルの名前。
	/// @param action 発生したファイルシステムアクションを示す数値。
	void FileWatcher::HandleAction(WatchStruct* watch, const String& fileName, unsigned long action)
	{
		Action fwAction;

		switch (action)
		{
		case FILE_ACTION_RENAMED_NEW_NAME:
		case FILE_ACTION_ADDED:
			fwAction = Actions::Add;
			break;

		case FILE_ACTION_RENAMED_OLD_NAME:
		case FILE_ACTION_REMOVED:
			fwAction = Actions::Delete;
			break;
		case FILE_ACTION_MODIFIED:
			fwAction = Actions::Modified;
			break;
		}

		watch->m_pFileWatchListener->handleFileAction(watch->m_Watchid, watch->m_DirName, fileName, fwAction);
	}

}//namespace FW