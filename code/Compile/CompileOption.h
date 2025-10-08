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

/// @brief コンパイル時に利用可能な最適化レベルを定義します。
enum OptimizationLevel
{
    OPTIMIZATIONLEVEC_DEFAULT = 0,  // デフォルト : DEBUGではdebug、リリースではPERF。これがデフォルトの状態です。
    OPTIMIZATIONLEVEL_DEBUG,        // デバッグ : 低い最適化、デバッグ体験を向上させます
    OPTIMIZATIONLEVEL_PERF,         // リリース : パフォーマンス用の最適化、デバッグ体験が劣る可能性があります
    OPTIMIZATIONLEVEL_NOT_SET,      // コンパイル時に最適化が設定されていない、基盤となるコンパイラのデフォルトかSetAdditionalCompileOptionsで設定
    OPTIMIZATIONLEVEL_SIZE,         // 列挙型のサイズ、最適化レベルの設定には使用しないでください
};

/// @brief 最適化レベルを表す文字列の配列です。
static const char* OptimizationLevelStrings[] =
{
    "DEFAULT",
    "DEBUG",
    "PERF",
    "NOT_SET",
};

/// @brief 実際の最適化レベルを取得します。OPTIMIZATIONLEVEC_DEFAULTが指定された場合、ビルド構成に基づいて適切なレベルに変換されます。
inline OptimizationLevel GetActualOptimizationLevel(OptimizationLevel optimizationLevel)
{
    // OPTIMIZATIONLEVEC_DEFAULTの場合、ビルド構成に基づいて適切なレベルに変換
    if (OPTIMIZATIONLEVEC_DEFAULT == optimizationLevel)
    {
    #ifdef _DEBUG
        optimizationLevel = OPTIMIZATIONLEVEL_DEBUG;
    #else
        optimizationLevel = OPTIMIZATIONLEVEL_PERF;
    #endif
    }

    // それ以外の場合はそのまま返す
    return optimizationLevel;
}
