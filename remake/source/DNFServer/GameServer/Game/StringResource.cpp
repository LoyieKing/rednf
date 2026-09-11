// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 StringResource/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— StringResource / CStringManager 实现（报告依据见 .h 文件头）。
// 逐段转写自 ORIG objdump（地址见各函数注释）；static helper
// parseStringLine（ORIG t _ZL15parseStringLinePcPiPS_ @0x08ad490f）同 TU。  

#include "StringResource.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <new>
#include <vector>

#include "StringResource/StringResource.inc"
#include "StringResource/dtor_StringResource.inc"


#include "StringResource/add.inc"


#include "StringResource/set.inc"


#include "StringResource/get.inc"


#include "StringResource/loadFromString.inc"


#include "StringResource/loadFromResource.inc"


#include "StringResource/loadFromBinary.inc"


#include "StringResource/loadFromFile.inc"


// ---------------------------------------------------------------------------
// app::stringResource —— ORIG T 0x08ad5439：guard 单例 + atexit 析构注册。
// C++98 局部静态生命周期与 ORIG guard/atexit 组合等价。
// ---------------------------------------------------------------------------
namespace app {

StringResource* stringResource()
{
    static StringResource sr;
    return &sr;
}

}  // namespace app

// STRING_FILE —— ORIG 单元内静态 _ZL11STRING_FILE（0x093f567c），恒 0。
static const char* STRING_FILE = 0;

#include "StringResource/loadString.inc"


// ---------------------------------------------------------------------------
// 编码原语。
//
// b28c-L8：ORIG _Z19convertToCharStringPKw @0x08adf54c 体为
// `mov &_ZL9gCodePage,%edx; mov %edx,0x8(%esp)`（内存读传参，非立即数）
// ⇒ 本 TU 有单元内 static 码页全局（与 StringConvert.cpp/GameScriptEnumHelpers.cpp
// 的同名 TU-local 各自独立，链接期不冲突）。旧形写常量 0 使真码页变更失效。
// ---------------------------------------------------------------------------
// convertToCharString(PKwj) —— ORIG T 0x08adf57f：wcstombs(NULL,src,0) 求长；
// 返回 (size_t)-1 或 0 → 返回空 CharString；否则 CharStringData::create(n)
// 缓冲内 wcstombs 写入并以 CharString(CharStringData*) 归还。
// convertToCharString(PKw)  —— ORIG T 0x08adf54c：转发 PKwj（ORIG 经单元内
// 码页全局传参，非立即数）。
// convertToCharString(RK10WideString) —— ORIG T 0x08adf624：sret，转发 c_str()。
// ---------------------------------------------------------------------------
// [b28d-Main R3 迁移·检查点13] gCodePage + convertToCharString(PKwj/PKw/RK10WideString)
// 已迁入 StringConvert.cpp（ORIG 归属 TU）；本 TU 不再持有码页副本。

#include "StringResource/convertFromUtf8.inc"


#include "StringResource/getStr.inc"


// ---------------------------------------------------------------------------
// CStringManager::getStr —— ORIG T 0x08ac35f8。
// 【验收 batch-11 尾项】orig=66 / our=66（gap 0）：errorMsg 改回 ORIG 的
// `static __thread char[32]`（%gs+lea 两段重算、无中间槽）+ ss_sprintf<32>
// 模板（W 定义/force 归 OnlyOrigBatch12.cpp，本 TU §8.1 声明）；is_sr_loaded
// 仅成功路径写位（旧实现无条件 `= loaded` 已纠正）；loaded 具名 bool 跨 err
// 析构存活。残差=4.1 把 loaded/return 聚合于 callee-saved %esi/%ebx 而 4.4
// 用栈槽（N82/N7 族），助记符行集等长。
// is_sr_loaded（ORIG _ZL12is_sr_loaded @0x0950189a）未置位时先加载一次；
// 加载失败返回 NULL（ORIG ebx=0 路径）；命中取表项 c_str()；未命中 sprintf
// "getStr(%d) == none"（格式串 @0x8e2e793）到静态 errorMsg 缓冲并返回。
// errorMsg 对应 ORIG getStr 内静态缓冲（bss 块 0x508）。
// ---------------------------------------------------------------------------
static bool is_sr_loaded = false;

// 模板版 ss_sprintf<N>（ORIG W _Z10ss_sprintfILj32EEiRAT__cPKcz 等）：定义与
// force 实例化归 OnlyOrigBatch12.cpp（属主批次），本 TU 按 §8.1 仅持等价声明，
// 链接期绑定其 W 实例。签名与 mangled 逐字对齐（RAT__c = char(&)[N]）。
template <size_t N>
#include "StringResource/ss_sprintf.inc"

#include "StringResource/getStr_1.inc"


#include "StringResource/setStringResourceLoaded.inc"

