// RuntimeCompiledC++を参考にしたコンパイルシステム
// https://github.com/RuntimeCompiledCPlusPlus/RuntimeCompiledCPlusPlus.git

//
// Copyright (c) 2017 Doug Binks
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

#pragma once

// 追跡に使用されるテンプレートは最適化する必要がないため、
// これを処理するためのマクロを作成します
#if defined _WIN32 && !defined __clang__
#define RCCPP_OPTMIZE_OFF __pragma( optimize( "", off ) )
#define RCCPP_OPTMIZE_ON  __pragma( optimize( "", on ) )
#else
#define RCCPP_OPTMIZE_OFF
#define RCCPP_OPTMIZE_ON
#endif

// ソース依存関係は、__FILE__マクロを含む可能性のあるソースからのマクロテンプレートから
// 構築されるため、相互依存を減らすために3つの値を返します。これらは上位レベルのコードで
// 結合されます。完全なソース依存ファイル名は疑似コードで以下のようになります:
// RemoveAnyFileName( relativeToPath ) + ReplaceExtension( filename, extension  )
struct SourceDependencyInfo
{
    const char* filename;			// If NULL then no SourceDependencyInfo
    const char* extension;			// If NULL then use extension in filename
    const char* relativeToPath;		// If NULL filename is either full or relative to known path
};

struct RuntimeTrackingInfo {
    static RuntimeTrackingInfo GetNULL() { return RuntimeTrackingInfo(); }
    SourceDependencyInfo m_SourceDependencyInfo;
    const char* m_LinkLibrary;
    const char* m_IncludeFile;
};

#include <stddef.h>

RCCPP_OPTMIZE_OFF

struct RuntimeTracking {
    RuntimeTracking(size_t max) :m_MaxNum(max){}

    ~RuntimeTracking() {}

    RuntimeTrackingInfo GetTrackingInfo(size_t num) const;
    size_t m_MaxNum;
};
