#pragma once

// RuntimeCompiledC++を参考にしたコンパイルシステム
// https://github.com/RuntimeCompiledCPlusPlus/RuntimeCompiledCPlusPlus.git

/**
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

#include <string>
#include <vector>

#include "CompileFileSystem.h"
#include "CompileOption.h"

class PlatformCompilerData;
struct CompilerLogger;

struct CompilerOptions
{
    std::vector<CompileFileSystem::Path>    includeDirList;         // インクルードディレクトリのリスト
    std::vector<CompileFileSystem::Path>    libraryDirList;         // ライブラリディレクトリのリスト
    std::string                             compileOptions;         // コンパイルオプション
    std::string                             linkOptions;            // リンクオプション
    OptimizationLevel                       optimizationLevel;      // 最適化レベル
    CompileFileSystem::Path                 baseIntermediatePath;   // 基本的な中間ファイルのパス
    CompileFileSystem::Path                 intermediatePath;       // 中間ファイルのパス
    CompileFileSystem::Path                 compilerLocation;       // コンパイラの場所
};

class Compiler
{
    PlatformCompilerData* m_pData;
    bool m_isFastCompile;
public:
    Compiler();
    ~Compiler();

    void Initialize(CompilerLogger* pLogger);

    // Windowsではコンパイルコマンドラインプロセスをコンパイル間で保持してパフォーマンスを向上させることができます。
    // ただし、これによりゾンビプロセスが発生したり、ソケットなどのハンドルが閉じられなくなる可能性があります。
    // この関数はいつでも安全に呼び出すことができますが、Win32コンパイルでは2回目以降のコンパイルから効果が現れます（最初のコンパイルではプロセスを起動してVS環境を設定する必要があるため）。
    // デフォルトではm_isFastCompile = falseです。
    void SetFastCompileMode(bool isFast)
    {
        m_isFastCompile = isFast;

        if (m_pData)
        {
            // GetIsComplete()を呼び出してプロセスを停止させる
            GetIsComplete();
        }
    }

    std::string GetObjectFileExtension() const;

    /// @brief 指定されたファイル群をコンパイルし、ライブラリをリンクしてモジュールを生成します。
    /// @param filesToCompile コンパイル対象となるファイルのパスのリスト。
    /// @param options コンパイラのオプション設定。
    /// @param linkLibraryList リンクするライブラリのパスのリスト。
    /// @param moduleName 生成されるモジュールのパス。
    void RunCompile(const std::vector<CompileFileSystem::Path>& filesToCompile,
                    const CompilerOptions& options,
                    const std::vector<CompileFileSystem::Path>& linkLibraryList,
                    const CompileFileSystem::Path& moduleName);

    bool GetIsComplete() const;
};

