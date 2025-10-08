// RuntimeCompiledC++を参考にしたコンパイルシステム
// https://github.com/RuntimeCompiledCPlusPlus/RuntimeCompiledCPlusPlus.git

/**
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


#pragma once

#include <string>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#ifdef _WIN32
	#include <direct.h>
	#include <sys/utime.h>

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
	#define NOMINMAX
#endif

	#include <windows.h>
	#undef GetObject
	#undef GetCurrentTime

	#define FILESYSTEMUTILS_SEPERATORS "/\\"

#else
	#include <utime.h>
	#include <string.h>
	#include <unistd.h>
	#include <dirent.h>
	#include <assert.h>
#define FILESYSTEMUTILS_SEPERATORS "/"

#endif // _WIN32

namespace CompileFileSystem
{
#ifdef _WIN32
	typedef __time64_t filetime_t;
#else
	typedef time_t filetime_t;
#endif // _WIN32

	class Path
	{
	public:
		std::string m_String;

		Path() {}

		Path(const std::string& rhs_)
			: m_String(rhs_)
		{
		}

		Path(const char* rhs_)
			: m_String(rhs_)
		{
		}

		virtual ~Path() {}

		const char* c_str()             const;

		Path& operator=(const std::string& rhs_);
		Path& operator=(const char* rhs_);

		bool        Exists()            const;
		bool        IsDirectory()       const;
		bool        CreateDir()         const;
		bool        Remove()            const;
		bool        RemoveDir()         const;
		filetime_t  GetLastWriteTime()  const;
		void        SetLastWriteTime(filetime_t time_) const;
		uint64_t    GetFileSize()       const;
		bool        HasExtension()      const;
		bool        HasParentPath()     const;
		std::string Extension()         const;

		// ファイル名を取得
		// part after final seperator
		// ./dirone/hello.txt > hello.txt ;
		// ./dirone/hello > hello ;
		// ./dirone/ > [empty string]
		Path        FileName()          const;
		Path        ParentPath()        const;
		Path        DelimitersToOSDefault() const;

		// /../ を含むパスから、直前のディレクトリを取り除いて正規化したパスを返す
		Path        GetCleanPath()      const;
		void        ToOSCanonicalCase(); // Windowsではlower case

		// Windows で UTF-8 や長いファイル名の fopen 用
		Path        GetOSShortForm()    const;

		bool Rename(Path newName_);

		// 拡張子を置き換える。ext は . を含む
		void ReplaceExtension(const std::string& ext);

#ifdef _WIN32
		static const char seperator = '\\';
		static const char altseperator = '/';
#else
		static const char seperator = '/';
		static const char altseperator = '\\';
#endif // _WIN32

	};

#ifdef _WIN32
	inline std::string _Win32Utf16ToUtf8(const std::wstring& wstr)
	{
		std::string convertedString;
		int requiredSize = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, 0, 0, 0, 0);
		if (requiredSize > 0)
		{
			convertedString.resize(requiredSize);
			WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &convertedString[0], requiredSize, 0, 0);
			convertedString.pop_back(); //remove NULL terminator
		}
		return convertedString;
	}

	inline std::wstring _Win32Utf8ToUtf16(const std::string& str)
	{
		std::wstring convertedString;
		int requiredSize = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, 0, 0);
		if (requiredSize > 0)
		{
			convertedString.resize(requiredSize);
			MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &convertedString[0], requiredSize);
			convertedString.pop_back(); //remove NULL terminator
		}

		return convertedString;
	}

#endif // _WIN32

	inline FILE* fopen(Path path, const char* mode)
	{
#ifdef _WIN32
		std::wstring wideStr = _Win32Utf8ToUtf16(path.m_String);
		std::wstring wideMode = _Win32Utf8ToUtf16(mode);
		FILE* fp;
		_wfopen_s(&fp, wideStr.c_str(), wideMode.c_str());
		return fp;
#else
		return ::fopen(path.m_String.c_str(), mode);
#endif // _WIN32

	}

	inline Path GetCurrentPath()
	{
		Path currentPath;
#ifdef _WIN32
		std::wstring temp;
		DWORD requiredSize = GetCurrentDirectoryW(0, NULL);
		if (requiredSize > 0)
		{
			temp.resize(requiredSize);
			GetCurrentDirectoryW(requiredSize, &temp[0]);
			temp.pop_back();
		}
		currentPath = _Win32Utf16ToUtf8(temp);
#else
		char* currentDir = getcwd(0, 0);
		currentPath = currentDir;
		free(currentDir);
#endif // _WIN32

		return currentPath;
	}

	inline Path GetExePath()
	{
		Path currPath;
#ifdef _WIN32
		std::wstring temp;
		int strSize = 25;
		temp.resize(strSize);
		int stringLength = 0;

		while (true)
		{
			stringLength = GetModuleFileNameW(NULL, &temp[0], strSize);
			if (stringLength != strSize)
			{
				break;
			}
			else {
				strSize *= 2;
				stringLength = 0;
				temp.resize(strSize);
			}
		}
		temp.resize(stringLength + 1);
		currPath = _Win32Utf16ToUtf8(temp);
#else
		assert(false);
#endif // _WIN32

		return currPath;

	}

	/// @brief 指定された文字列内のすべての英大文字を小文字に変換します（インプレースで実行）。
	/// @param inout_str 小文字に変換される対象の文字列。変換結果はこの引数に直接反映されます。
	inline void ToLowerInPlace(std::string& inout_str)
	{
		for (size_t i = 0; i < inout_str.size(); ++i)
		{
			if (inout_str[i] >= 'A' && inout_str[i] <= 'Z')
			{
				inout_str[i] -= 'A' - 'a';
			}
		}
	}

	/// @brief 現在の時刻を取得します。
	/// @return 現在の時刻を表す filetime_t 型の値。
	inline filetime_t GetCurrentTime()
	{
		filetime_t timer;
#ifdef _WIN32
		_time64(&timer);
#else
		time(&timer);
#endif // _WIN32
		return timer;
	}

	/// @brief 指定された filetime_t 値から UTC 時間の tm 構造体を取得します。
	/// @param time 変換する filetime_t 型の時刻値。
	/// @return 指定した時刻を表す tm 構造体。
	inline tm GetTimeStruct(filetime_t time)
	{
		tm ret;
#ifdef _WIN32
		_gmtime64_s(&ret, &time);
#else
		gmtime_r(&time, &ret);
#endif // _WIN32
		return ret;
	}

	/// @brief 指定されたファイル時刻からローカル時刻の tm 構造体を取得します。
	/// @param time 変換するファイル時刻（filetime_t 型）。
	/// @return ローカル時刻を表す tm 構造体。
	inline tm GetLocalTimeStruct(filetime_t time)
	{
		tm ret;
#ifdef _WIN32
		_localtime64_s(&ret, &time);
#else
		localtime_r(&time, &ret);
#endif // _WIN32
		return ret;
	}

	///////////////////////////////////////////////////////////////////
	// Path function definitions

	inline const char* Path::c_str() const
	{
		return m_String.c_str();
	}

	inline Path& Path::operator=(const std::string& rhs_)
	{
		m_String = rhs_;
		return *this;
	}

	inline Path& Path::operator=(const char* rhs_)
	{
		m_String = rhs_;
		return *this;
	}

	/// @brief パスが存在するかどうかを判定します。
	/// @return パスが存在する場合は true、存在しない場合は false を返します。
	inline bool Path::Exists() const
	{
		int error = -1;
#ifdef _WIN32
		struct _stat64 buffer;
		std::wstring temp;
		//special handling for drives on Windows
		if (m_String.size() == 2 && m_String[1] == ':')
		{
			temp = _Win32Utf8ToUtf16(m_String + "\\");
		}
		else {
			temp = _Win32Utf8ToUtf16(m_String);
		}
		error = _wstat64(temp.c_str(), &buffer);
#else
		struct stat buffer;
		error = stat(m_String.c_str(), &buffer);
#endif // _WIN32
		if (0 == error)
		{
			return true;
		}
		return false;
	}

	/// @brief パスがディレクトリかどうかを判定します。
	/// @return パスがディレクトリの場合は true、そうでない場合は false を返します。
	inline bool Path::IsDirectory() const
	{
		int error = -1;
		bool isDir = false;
#ifdef _WIN32
		struct _stat64 buffer;
		std::wstring temp = _Win32Utf8ToUtf16(m_String);
		error = _wstat64(temp.c_str(), &buffer);
#else
		struct stat buffer;
		error = stat(m_String.c_str(), &buffer);
#endif
		if (0 == error)
		{
			isDir = 0 != (buffer.st_mode & S_IFDIR);
		}
		return isDir;
	}

	/// @brief ディレクトリが存在しない場合に、パスで指定されたディレクトリを（親ディレクトリも含めて）作成します。
	/// @return ディレクトリの作成に成功した場合は true、失敗した場合は false を返します。
	inline bool Path::CreateDir() const
	{
		if (m_String.length() == 0)
		{
			return false;
		}
		if (Exists())
		{
			return false;
		}

		// we may need to create the parent path recursively
		Path parentpath = ParentPath();
		if (!parentpath.Exists())
		{
			parentpath.CreateDir();
		}

		int error = -1;
#ifdef _WIN32
		std::wstring temp = _Win32Utf8ToUtf16(m_String);
		error = _wmkdir(temp.c_str());
#else
		error = mkdir(m_String.c_str(), 0777);
#endif // _WIN32
		if (0 == error)
		{
			return true;
		}
		return false;
	}

	/// @brief ファイルまたはパスの最終更新時刻を取得します。
	/// @return ファイルまたはパスの最終更新時刻（エポック秒、失敗時は0）。
	inline filetime_t Path::GetLastWriteTime() const
	{
		filetime_t lastWriteTime = 0;
		int error = -1;
#ifdef _WIN32
		struct _stat64 buffer;
		std::wstring temp = _Win32Utf8ToUtf16(m_String);
		error = _wstat64(temp.c_str(), &buffer);
#else
		struct stat buffer;
		error = stat(c_str(), &buffer);
#endif // _WIN32
		if (0 == error)
		{
			lastWriteTime = buffer.st_mtime;
		}
		return lastWriteTime;
	}

	/// @brief ファイルの最終更新時刻を設定します。
	/// @param time_ 設定する新しい最終更新時刻（filetime_t 型）。
	inline void Path::SetLastWriteTime(filetime_t time_) const
	{
#ifdef _WIN32
		__utimbuf64 modtime = { time_, time_ };
		std::wstring temp = _Win32Utf8ToUtf16(m_String);
		_wutime64(temp.c_str(), &modtime);
#endif // _WIN32

	}

	/// @brief パスで指定されたファイルまたはディレクトリを削除します。
	/// @return 削除に成功した場合は true、失敗した場合は false を返します。
	inline bool Path::Remove() const
	{
#ifdef _WIN32
		std::wstring temp = _Win32Utf8ToUtf16(m_String);
		int error = _wremove(temp.c_str());
#endif
		if (!error)
		{
			return true;
		}
		return false;
	}

	// RemoveDir -  The directory must be empty, and it must not be the current working directory or the root directory.
	/// @brief ディレクトリを削除します。
	/// @return ディレクトリの削除に成功した場合は true、失敗した場合は false を返します。
	inline bool Path::RemoveDir() const
	{
#ifdef _WIN32
		std::wstring temp = _Win32Utf8ToUtf16(m_String);
		int error = _wrmdir(temp.c_str());
#endif
		if (!error)
		{
			return true;
		}
		return false;
	}

	/// @brief パス名を新しい名前に変更します。
	/// @param newName_ 新しいパス名を指定するPathオブジェクト。
	/// @return リネームに成功した場合はtrue、失敗した場合はfalseを返します。
	inline bool Path::Rename(Path newName_)
	{
#ifdef _WIN32
		std::wstring oldName = _Win32Utf8ToUtf16(m_String);
		std::wstring newName = _Win32Utf8ToUtf16(newName_.m_String);
		int error = _wrename(oldName.c_str(), newName.c_str());
#endif // _WIN32

		if (!error)
		{
			m_String = newName_.m_String;
			return true;
		}
		return false;
	}

	/// @brief ファイルのサイズ（バイト単位）を取得します。
	/// @return ファイルのサイズ（バイト単位）。ファイルが存在しない場合は0を返します。
	inline uint64_t Path::GetFileSize() const
	{
		uint64_t fileSize = 0;
		int error = -1;
#ifdef _WIN32
		struct _stat64 buffer;
		std::wstring temp = _Win32Utf8ToUtf16(m_String);
		error = _wstat64(temp.c_str(), &buffer);
#endif // _WIN32
		if (0 == error)
		{
			fileSize = buffer.st_size;
		}
		return fileSize;
	}

	/// @brief パス文字列に拡張子が含まれているかどうかを判定します。
	/// @return 拡張子が存在する場合は true、存在しない場合は false を返します。
	inline bool Path::HasExtension() const
	{
		size_t posDot = m_String.find_last_of('.');
		if (posDot != std::string::npos)
		{
			size_t posSeperator = m_String.find_last_of(seperator);
			if (posSeperator != std::string::npos && posSeperator > posDot)
			{
				return false; // the dot is for a directory such as ./ or ../ or the path is malformed
			}
			size_t posAltSeperator = m_String.find_last_of(altseperator);
			if (posAltSeperator != std::string::npos && posAltSeperator > posDot)
			{
				return false; // the dot is for a directory such as ./ or ../ or the path is malformed
			}
			return true;
		}
		return false;
	}

	/// @brief パスが親ディレクトリを持つかどうかを判定します。
	/// @return 親ディレクトリが存在する場合は true、存在しない場合は false を返します。
	inline bool Path::HasParentPath() const
	{
		size_t posSeperator = m_String.find_last_of(FILESYSTEMUTILS_SEPERATORS);
		if (posSeperator != std::string::npos && posSeperator > 0)
		{
			if (posSeperator >= 1 && m_String[posSeperator - 1] == ':' && posSeperator == m_String.length() - 1)
			{
				return false;
			}
			return true;
		}
		size_t posAltSeperator = m_String.find_last_of(altseperator);
		if (posAltSeperator != std::string::npos && posAltSeperator > 0)
		{
			return true;
		}
		return false;
	}

	/// @brief パス文字列から拡張子を取得します。
	/// @return 拡張子を含む文字列（例: ".txt"）。拡張子が存在しない場合は空文字列を返します。
	inline std::string Path::Extension() const
	{
		std::string ext;
		if (HasExtension())
		{
			size_t pos = m_String.find_last_of('.');
			if (pos < m_String.length())
			{
				ext = m_String.substr(pos);
			}
		}
		return ext;
	}

	/// @brief パスからファイル名部分を抽出して返します。
	/// @return パスのファイル名部分を表す新しい Path オブジェクト。
	inline Path Path::FileName() const
	{
		Path fileName;
		size_t pos = m_String.find_last_of(seperator) + 1;
		if (pos <= m_String.length())
		{
			if (m_String.length() == 3 && m_String[1] == ':' && pos == 3)
			{
				fileName = m_String;
				return fileName;
			}
			fileName = m_String.substr(pos);
		}
		return fileName;
	}

	/// @brief パス文字列の末尾にある区切り文字を削除します。
	/// @param path 末尾の区切り文字を削除する Path オブジェクトへの参照。
	inline void RemoveTrailingSeperators(Path& path)
	{
		while (path.m_String.find_last_of(FILESYSTEMUTILS_SEPERATORS) == path.m_String.length() - 1)
		{
			if (path.m_String.length() == 3 && path.m_String[1] == ':')
			{
				return;
			}
			path.m_String.erase(path.m_String.length() - 1, 1);
		}
	}

	/// @brief パスの親ディレクトリを取得します。
	/// @return このパスの親ディレクトリを表す Path オブジェクト。親ディレクトリが存在しない場合は空のパスを返します。
	inline Path Path::ParentPath() const
	{
		Path parentPath = m_String;

		if (parentPath.m_String.length() == 0)
		{
			return parentPath;
		}
		RemoveTrailingSeperators(parentPath);

		size_t pos = parentPath.m_String.find_last_of(FILESYSTEMUTILS_SEPERATORS);
		if (pos < parentPath.m_String.length())
		{
			if (pos >= 2 && parentPath.m_String[pos - 1] == ':')
			{
				++pos;
			}
			parentPath = parentPath.m_String.substr(0, pos);

			// すべての区切り文字を削除
			RemoveTrailingSeperators(parentPath);
		}
		else
		{
			// 区切り文字がないときは親ディレクトリが空
			parentPath = "";
		}

		return parentPath;
	}

	/// @brief パス区切り文字をOSのデフォルト区切り文字に変換した新しいPathオブジェクトを返します。
	/// @return 区切り文字がOSのデフォルトに置き換えられた新しいPathオブジェクト。
	inline Path Path::DelimitersToOSDefault() const
	{
		Path path = m_String;
		for (size_t i = 0; i < path.m_String.size(); ++i)
		{
			if (path.m_String[i] == altseperator)
			{
				path.m_String[i] = seperator;
			}
		}
		return path;
	}

	/// @brief パスの拡張子を指定された拡張子で置き換えます。
	/// @param ext 新しい拡張子（ドットを含む場合も含まない場合もあります）。
	inline void Path::ReplaceExtension(const std::string& ext)
	{
		if (HasExtension())
		{
			size_t pos = m_String.find_last_of('.');
			if (pos < m_String.length())
			{
				m_String.erase(m_String.begin() + pos, m_String.end());
			}
		}
		m_String += ext;
	}

	/// @brief 2つのPathオブジェクトを結合し、新しいパスを作成する演算子/のオーバーロードです。
	/// @param lhs 結合する左側のPathオブジェクト。
	/// @param rhs 結合する右側のPathオブジェクト。
	/// @return 結合された新しいPathオブジェクト。
	inline Path operator/(const Path& lhs, const Path& rhs)
	{
		if (0 == lhs.m_String.length())
		{
			return rhs;
		}
		if (0 == rhs.m_String.length())
		{
			return lhs;
		}
		std::string strlhs = lhs.m_String;
		while (strlhs.length() && strlhs.find_last_of(FILESYSTEMUTILS_SEPERATORS) == strlhs.length() - 1)
		{
			strlhs.erase(strlhs.length() - 1, 1);
		}

		//note:rhs_ の先頭にある区切り文字（セパレータ）は本来取り除くべきだが、現時点では未対応
		Path join = strlhs + Path::seperator + rhs.m_String;
		return join;
	}

	inline bool operator == (const Path& lhs, const Path& rhs)
	{
		return lhs.m_String == rhs.m_String;
	}

	inline bool operator < (const Path& lhs, const Path& rhs)
	{
		return lhs.m_String < rhs.m_String;
	}

	inline Path Path::GetCleanPath() const
	{
		Path path = m_String;
		bool isFound = false;
		do
		{
			isFound = false;
			size_t pos = path.m_String.find("..");
			if (pos != std::string::npos && pos + 3 < path.m_String.length() && pos > 0)
			{
				Path a = path.m_String.substr(0, pos - 1);                      // 区切り文字（デリミタ）は不要なので pos-1 としている
				Path b = path.m_String.substr(pos + 3, path.m_String.length()); // 区切り文字（デリミタ）は不要なので pos+3 としている
				a = a.ParentPath();
				path = a / b;
				isFound = true;
			}
		} while (isFound);

		return path;
	}

    inline void Path::ToOSCanonicalCase()
    {
#ifdef _WIN32
        ToLowerInPlace(m_String);
#endif // _WIN32
    }

    inline Path Path::GetOSShortForm() const
    {
#ifdef _WIN32
        std::wstring longForm = _Win32Utf8ToUtf16(m_String);
        std::wstring shortForm;
        DWORD requiredSize = GetShortPathNameW(longForm.c_str(), NULL, 0);
        if (requiredSize > 0)
        {
            shortForm.resize(requiredSize);
            GetShortPathNameW(longForm.c_str(), &shortForm[0], requiredSize);
            shortForm.pop_back();
            return _Win32Utf16ToUtf8(shortForm);
        }
        return m_String;
#else
        return *this;
#endif // _WIN32
    }

    class PathIterator
    {
    private:
        Path m_Dir;
        Path m_Path;
        bool m_IsValid;
#ifdef _WIN32
        HANDLE m_hFind;
        WIN32_FIND_DATAW m_FFD;
        /// @brief ディレクトリ内のファイル検索を初期化します。
        void ImpCtor()
        {
            Path test = m_Dir / "*";
            m_Path = m_Dir;
            m_hFind = INVALID_HANDLE_VALUE;
            std::wstring temp = _Win32Utf8ToUtf16(test.m_String);
            m_hFind = FindFirstFileW(temp.c_str(), &m_FFD);
            m_IsValid = INVALID_HANDLE_VALUE != m_hFind;
        }
        /// @brief ディレクトリ検索で次のファイルまたはディレクトリエントリに進みます。
        /// @return 次の有効なエントリが見つかった場合は true、見つからなかった場合は false を返します。
        bool ImpNext()
        {
            if (m_IsValid)
            {
                m_IsValid = 0 != FindNextFileW(m_hFind, &m_FFD);
                if (m_IsValid)
                {
                    std::string temp = _Win32Utf16ToUtf8(m_FFD.cFileName);
                    m_Path = m_Dir / temp;
                    if (m_Path.FileName() == "..")
                    {
                        return ImpNext();
                    }
                }
            }
            return m_IsValid;
        }
        /// @brief m_hFind ハンドルを閉じるためのデストラクタ補助関数です。
        void ImpDtor()
        {
            FindClose(m_hFind);
        }
#else
        void ImpCtor()
        {
            Path test = m_dir / "*";
            m_path = m_dir;
            m_numFilesInList = scandir(m_path.c_str(), &m_paDirFileList, 0, alphasort);
            m_bIsValid = m_numFilesInList > 0;
            m_currFile = 0;
            if (!m_bIsValid)
            {
                m_paDirFileList = 0;
            }
        }
        bool ImpNext()
        {
            if (m_bIsValid)
            {
                ++m_currFile;
                m_bIsValid = m_currFile < m_numFilesInList;
                if (m_bIsValid)
                {
                    m_path = m_dir / m_paDirFileList[m_currFile]->d_name;
                    if (strcmp(m_paDirFileList[m_currFile]->d_name, ".") == 0 ||
                        strcmp(m_paDirFileList[m_currFile]->d_name, "..") == 0)
                    {
                        return ImpNext();
                    }
                }
            }
            return m_bIsValid;
        }
        void ImpDtor()
        {
            free(m_paDirFileList);
        }
        struct dirent** m_paDirFileList;
        int           m_numFilesInList;
        int           m_currFile;

#endif // _WIN32
    public:
        PathIterator(const Path& path)
            : m_Dir(path)
        {
            ImpCtor();
        }

        ~PathIterator()
        {
            ImpDtor();
        }

        bool operator++()
        {
            return ImpNext();
        }

        bool IsValid() const
        {
            return m_IsValid;
        }
        const Path& GetPath() const
        {
            return m_Path;
        }
    };
}

inline CompileFileSystem::Path operator/(const std::string& lhs, const std::string& rhs)
{
    CompileFileSystem::Path lhsPath = lhs;
    CompileFileSystem::Path rhsPath = rhs;
    return lhsPath / rhsPath;
}
