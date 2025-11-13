//
// Copyright (c) 2013 Márton Tamás
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

// Minor modifications by Doug Binks
// This implementation for FileWatcherWin32 uses FindFirstChangeNotification rather
// than ReadDirectoryChanges

#pragma once

#include <iostream>
#include <vector>
#include <string>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace FW
{
    /// @brief ファイルシステムの変更通知タイプを表す定数の配列です。
    const static int FW_TYPES[] = {
        FILE_NOTIFY_CHANGE_LAST_WRITE,
        FILE_NOTIFY_CHANGE_CREATION,
        FILE_NOTIFY_CHANGE_SIZE,
        FILE_NOTIFY_CHANGE_FILE_NAME
    };
    /// @brief FW_TYPES 配列の要素数を表す定数です。
    const static size_t NUMTYPES = sizeof(FW_TYPES) / sizeof(int);

    /// @brief ファイルやディレクトリの変更を監視し、変更イベントを検出するクラスです。
    class FileWatcher_Alt
    {
        /// @brief ファイルの情報（ファイル名、サイズ、作成日時、最終更新日時）を格納するFileData構造体を初期化します。
        struct FileData
        {
            std::string fileName;
            LARGE_INTEGER fileSize;
            FILETIME fileTime;      // 作成時間
            FILETIME fileWriteTime; // 最終更新時間

            /// @brief ファイルの情報を格納するFileDataオブジェクトを初期化します。
            /// @param n ファイル名を表す文字列。省略時は空文字列になります。
            /// @param s ファイルサイズを表すLARGE_INTEGER型の値。省略時はデフォルト値になります。
            /// @param t ファイルの作成日時を表すFILETIME型の値。省略時はデフォルト値になります。
            /// @param tw ファイルの最終更新日時を表すFILETIME型の値。省略時はデフォルト値になります。
            FileData(std::string n = std::string(), LARGE_INTEGER s = LARGE_INTEGER(), FILETIME t = FILETIME(), FILETIME tw = FILETIME())
            {
                fileName = n;
                fileSize = s;
                fileTime = t;
                fileWriteTime = tw;
            }
        };

        /// @brief NUMTYPES 個の型ごとに、HANDLE の std::vector を格納する配列です。
        std::vector<HANDLE> handles[NUMTYPES];
        /// @brief ディレクトリ内のファイルデータを格納する2次元ベクターです。
        std::vector<std::vector<FileData>> directoryContents;

        /// @brief 指定したディレクトリ内のファイル一覧を取得し、各ファイルの情報を contents ベクターに格納します。
        /// @param path ファイル一覧を取得するディレクトリのパス。
        /// @param contents 取得したファイル情報を格納する FileData 構造体のベクター。
        void GetDirectoryContents(const std::string& path, std::vector<FileData>& contents)
        {
            // ファイル・ディレクトリ情報の構造体
            WIN32_FIND_DATAA fd;
            // ディレクトリ内のファイル列挙操作のためのハンドル
            HANDLE directoryLister = FindFirstFileA((path + "\\*").c_str(), &fd);

            if (directoryLister == INVALID_HANDLE_VALUE)
            {// 列挙操作の開始に失敗した場合
                std::cout << "Coundn't list files." << std::endl;
                return;
            }

            // ディレクトリ内のファイルを列挙
            do
            {
                // ディレクトリは除外し、ファイルのみを対象とする
                if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
                {
                    // ファイル情報を FileData 構造体に格納し、contents ベクターに追加
                    LARGE_INTEGER fileSize;
                    fileSize.LowPart = fd.nFileSizeLow;
                    fileSize.HighPart = fd.nFileSizeHigh;
                    FILETIME fileTime;
                    fileTime = fd.ftCreationTime;
                    FILETIME fileWriteTime;
                    fileWriteTime = fd.ftLastWriteTime;
                    contents.push_back(FileData(fd.cFileName, fileSize, fileTime, fileWriteTime));
                }

                // 次のファイルへ
            } while (FindNextFileA(directoryLister, &fd) != 0);

            // 列挙操作のハンドルを閉じる
            FindClose(directoryLister);
        }

    public:
        ~FileWatcher_Alt()
        {
            // すべての監視ハンドルを閉じる
            for (int c = 0; c < NUMTYPES; c++)
            {
                for (size_t d = 0; d != handles[c].size(); d++)
                {
                    FindCloseChangeNotification(handles[c][d]);
                }
            }
        }

        /// @brief ファイルやディレクトリの変更タイプを表す列挙型です。
        enum Type
        {
            CHANGE_LAST_WRITE = 0,
            CHANGE_CREATION,
            CHANGE_SIZE,
            CHANGE_FILE_NAME,
            CHANGE_NONE
        };

        /// @brief イベントを表す構造体です。
        struct FW_Event
        {
            size_t id;
            Type type;

            FW_Event(size_t i = -1, Type t = CHANGE_NONE) {
                id = i;
                type = t;
            }
        };

        /// @brief 指定されたパスのディレクトリ監視ハンドルを作成し、ディレクトリ内容を取得します。
        /// @param path 監視対象ディレクトリのパス。
        /// @return 作成されたディレクトリ監視ハンドルのID（size_t型）。エラーが発生した場合は-1を返します。
        size_t Add(const std::string& path)
        {
            size_t id = handles[0].size();

            // 各変更タイプごとに監視ハンドルを作成
            for (int c = 0; c < NUMTYPES; c++) {
                // 監視ハンドルを作成し、対応するベクターに追加
                handles[c].push_back(FindFirstChangeNotificationA(path.c_str(), false, FW_TYPES[c]));

                // ハンドルの作成に失敗した場合はエラーメッセージを表示して終了
                if (handles[c][id] == INVALID_HANDLE_VALUE)
                {
                    std::cerr << "Couldn't create handle." << std::endl;
                    return -1;
                }
            }

            // ディレクトリ内のファイル情報を取得し、directoryContents に格納
            directoryContents.resize(id + 1);
            GetDirectoryContents(path, directoryContents[id]);

            // 作成された監視ハンドルのIDを返す
            return id;
        }

        /// @brief 複数の監視ハンドルをチェックし、シグナル状態のイベントを検出してリストに追加します。
        /// @param ids シグナル状態となったイベント情報を追加するFW_Event型のベクター参照。
        /// @return すべての監視ハンドルの状態確認と次の通知設定が正常に完了した場合はtrue、失敗した場合はfalseを返します。
        bool Watch(std::vector<FW_Event>& ids)
        {
            // 各変更タイプごとに監視ハンドルをチェック
            for (int c = 0; c < NUMTYPES; c++)
            {
                // 監視ハンドルの状態を確認
                DWORD status = WaitForMultipleObjects(handles[c].size(), &handles[c][0], false, 0);
                // 監視ハンドルがシグナル状態になっている場合、対応するイベントをidsに追加
                for (size_t d = 0; d < handles[c].size(); d++)
                {
                    if (status == WAIT_OBJECT_0 + d)
                    {
                        ids.push_back(FW_Event(d, (Type)c));
                        // 次の変更通知を待機状態に戻す
                        if (FindNextChangeNotification(handles[c][d]) == false)
                        {
                            return false;
                        }
                    }
                }
            }
            return true;
        }

        /// @brief 指定されたディレクトリの内容と監視対象の内容を比較し、ファイルの変更イベント（作成、削除、名前変更、サイズ変更、タイムスタンプ変更）を検出して、該当するファイル名を返します。
        /// @param path 監視するディレクトリのパス。
        /// @param id 監視対象ディレクトリのインデックスまたは識別子。
        /// @param type 検出するイベントの種類（例: 変更、作成、名前変更、サイズ変更など）。
        /// @param fni 検出されたファイルイベントのアクション種別（参照渡しで返却）。
        /// @param oldName ファイル名変更イベント時の古いファイル名（参照渡しで返却）。
        /// @return 検出されたイベントに該当するファイル名。該当するイベントがない場合は空文字列を返します。
        std::string GetEventFileName(const std::string& path, size_t id, Type type, DWORD& fni, std::string& oldName)
        {
            static std::vector<FileData> contents;
            contents.clear();

            // 指定されたディレクトリの現在の内容を取得
            GetDirectoryContents(path, contents);

            // 新しいファイル名を格納する変数
            // CHANGE_FILE_NAME タイプのイベントで使用
            std::string fileName;

            switch (type)
            {
            // ファイルの最終更新日時の変更
            // 変更前の日時と変更後の日時を比較し、異なる場合にイベントを検出
            case CHANGE_LAST_WRITE:
                for (auto c = contents.begin(); c != contents.end(); c++)
                {
                    for (auto d = directoryContents[id].begin(); d != directoryContents[id].end(); d++)
                    {
                        if (c->fileName==d->fileName&&
                            (c->fileWriteTime.dwLowDateTime != d->fileWriteTime.dwLowDateTime ||
                             c->fileWriteTime.dwHighDateTime != d->fileWriteTime.dwHighDateTime))
                        {
                            // イベントを「中和」するために、変更前の日時を更新
                            d->fileWriteTime.dwLowDateTime = c->fileWriteTime.dwLowDateTime;
                            d->fileWriteTime.dwHighDateTime = c->fileWriteTime.dwHighDateTime;
                            fni = FILE_ACTION_MODIFIED;
                            return d->fileName;
                        }
                    }
                }
                break;
            // ファイルの作成日時の変更
            // 変更前の日時と変更後の日時を比較し、異なる場合にイベントを検出
            case CHANGE_CREATION:
                for (auto c = contents.begin(); c != contents.end(); c++)
                {
                    for (auto d = directoryContents[id].begin(); d != directoryContents[id].end(); d++)
                    {
                        if (c->fileName == d->fileName&&
                            (c->fileTime.dwLowDateTime != d->fileTime.dwLowDateTime ||
                             c->fileTime.dwHighDateTime != d->fileTime.dwHighDateTime))
                        {
                            // イベントを「中和」するために、変更前の日時を更新
                            d->fileTime.dwLowDateTime = c->fileTime.dwLowDateTime;
                            d->fileTime.dwHighDateTime = c->fileTime.dwHighDateTime;
                            fni = FILE_ACTION_ADDED;
                            return d->fileName;
                        }
                    }
                }
                break;
            // ファイル名の変更
            // 新しいファイル名を見つけて追加し、古いファイル名を削除
            case CHANGE_FILE_NAME:

            {
                // ファイルが削除されたかどうかを確認
                // ファイルが削除された場合、contents のサイズは directoryContents[id] のサイズより小さくなる
                bool isFileDeleted = contents.size() < directoryContents[id].size();

                // 新しいファイル名を見つけて追加
                for (auto c = contents.begin(); c != contents.end(); c++)
                {
                    bool found = false;

                    // 既存のファイル名と比較して、新しいファイル名を特定
                    for (auto d = directoryContents[id].begin(); d != directoryContents[id].end(); d++)
                    {
                        if (c->fileName == d->fileName)
                        {
                            found = true;
                            break;
                        }
                    }

                    // 新しいファイル名が見つかった場合、その名前を fileName に格納し、directoryContents[id] に追加
                    if (!found)
                    {
                        // これが新しいファイル名であることを確認
                        fileName = c->fileName;
                        directoryContents[id].push_back(*c); // 新しいファイルを追加
                        fni = FILE_ACTION_ADDED;
                        break;
                    }
                }

                // 古いファイル名を見つけて削除
                for (auto c = directoryContents[id].begin(); c != directoryContents[id].end(); c++)
                {
                    bool found = false;

                    // 現在の contents と比較して、存在しないファイル名を特定
                    for (auto d = contents.begin(); d != contents.end(); d++)
                    {
                        if (c->fileName == d->fileName)
                        {
                            found = true;
                            break;
                        }
                    }

                    // 存在しないファイル名が見つかった場合、その名前を返し、directoryContents[id] から削除
                    if (!found)
                    {
                        // これが古いファイル名である
                        if (isFileDeleted) // ファイルが削除された場合
                        {
                            fileName = c->fileName;
                            fni = FILE_ACTION_REMOVED;
                        }
                        else
                        {
                            oldName = c->fileName;
                            fni = FILE_ACTION_MODIFIED;
                        }
                        directoryContents[id].erase(c);
                        return fileName;
                    }
                }

                // ファイルが新規作成された場合
                if (!fileName.empty())
                {
                    fni = FILE_ACTION_ADDED;
                    return fileName;
                }

                break;
            }

            // ファイルサイズの変更
            // 変更前のサイズと変更後のサイズを比較し、異なる場合
            case CHANGE_SIZE:
                for (auto c = contents.begin(); c != contents.end(); c++)
                {
                    for (auto d = directoryContents[id].begin(); d != directoryContents[id].end(); d++)
                    {
                        if (c->fileName == d->fileName&&
                            (c->fileSize.LowPart != d->fileSize.LowPart ||
                             c->fileSize.HighPart != d->fileSize.HighPart))
                        {
                            // イベントを「中和」するために、変更前のサイズを更新
                            d->fileSize.LowPart = c->fileSize.LowPart;
                            d->fileSize.HighPart = c->fileSize.HighPart;
                            fni = FILE_ACTION_MODIFIED;
                            return d->fileName;
                        }
                    }

                    // 新しいファイル
                    for (auto c = contents.begin(); c != contents.end(); c++)
                    {
                        bool found = false;
                        for (auto d = directoryContents[id].begin(); d != directoryContents[id].end(); d++)
                        {
                            if (c->fileName == d->fileName)
                            {
                                found = true;
                                break;
                            }
                        }
                        if (!found)
                        {
                            // これが新しいファイル名であることを確認
                            fni = FILE_ACTION_ADDED;
                            return c->fileName;
                        }
                    }

                }
                break;
            default:
                break;
            }

            return "";
        }

        /// @brief 指定されたIDに対応するディレクトリ監視ハンドルと内容をすべてのタイプから削除します。
        /// @param id 削除するディレクトリエントリのインデックス。
        void Remove(size_t id)
        {
            // 各変更タイプごとに監視ハンドルを閉じ、対応するベクターから削除
            for (int c = 0; c < NUMTYPES; c++)
            {
                auto itHandles = handles[c].begin();
                auto itContents = directoryContents.begin();

                for (int d = 0; d < id; d++)
                {
                    itHandles++;
                    itContents++;
                };

                // 監視ハンドルを閉じ、ベクターから削除
                FindCloseChangeNotification(*itHandles);
                handles[c].erase(itHandles);
                directoryContents.erase(itContents);
            }
        }
    };
}
