// ============================================================================
// [SplitC batch9 · routeB_data 档2] g_script_package（ORIG TU = RDARScript.cpp）
//   目标键：_GLOBAL__I_g_script_package；构造序敏感（档2 之一）。
// ============================================================================
#include "PackSet.h"

PackSet g_script_package;


// ===== [SplitC batch9 · P2b merged from DNFLexWrapperHelpers.cpp] =====
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 DNFLexWrapperHelpers/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "DNFLexWrapper.h"

#include <cstdio>
#include <cstring>
#include <cstdarg>
#include <cmath>

#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "LogManager.h"
#include "RDARScriptStringManager.h"
#include "SyncScriptDeps.h"
#include "MySQL.h"   // GetEscapeString 调用 MySQL::escape_string（ORIG 0x0861cc47）
#include "PackSet.h"  // g_script_package（ORIG B 0x9494fe0）

extern PackSet g_script_package;            // OnlyOrigBatch12.cpp（ORIG B 0x09494fe0）
extern bool findFileWithServerToken(char* path);  // LinkMiscSecuTimerSolo.cpp（ORIG T 0x88bb7db）
#include "DNFLexWrapperHelpers/getScriptFileFullPath.inc"
                           const char* name);

namespace
{
// [AnonLane round8 2026-09-11 · 保留判词] 本块 5 条 only-NEW（g_tokens/g_nextToken/
// nextToken/parseIntToken/parseFloatToken，ORIG 全库零命中）**实测删除净损 ⇒ 保留**：
// 删除后本 TU 四数 identical 10→5、ae 46→32（合计 −19，missing 4→1）——与
// Dispatchers.cpp P4 负结论同型（P4：编译产物符号集是判据的一部分）：parseIntToken/
// parseFloatToken 内的 std::string 临时/strtol/strtod 与 vector<string> 实例在本 .o 的
// 发射点被 compare 计入 identical/AE，删除即蒸发。ORIG 侧对应实现位于
// `_ZN13DNFLexWrapper*` 成员族（nm 实测 0 个 TU-local token helper）。
std::vector<std::string> g_tokens;
std::size_t g_nextToken = 0;

#include "DNFLexWrapperHelpers/nextToken.inc"


#include "DNFLexWrapperHelpers/parseIntToken.inc"


#include "DNFLexWrapperHelpers/parseFloatToken.inc"

}

// 加载回调（ORIG 0x0949b2b4 B：loadRDARScriptFile 在切换输入流前若非空则无参调用）
void (*g_onLoadScriptFileFunction)() = 0;

// ORIG B 0x0949b29c [0x18]：脚本字符串文件列表（STScriptFileList，sizeof 0x18）。
// 归属本 TU 的证据：①地址与上方 g_onLoadScriptFileFunction（0x0949b2b4）紧邻
// （0x0949b29c + 0x18 = 0x0949b2b4）；②ORIG 侧唯一动态引用即本 TU 的
// __static_initialization_and_destruction_0ii @0x088c3145
// `movl $0x949b29c,(%esp); call STScriptFileList::C1Ev`（0x088bacf8）
// + __cxa_atexit(D1 0x80f1352)（objdump -d 全库无其它读点）。
// [Round7 FillC] 补数据符号定义（ORIG nm: B 0x18，逐位对账；nm 我方原缺此符号）。
#include "NpcScript.h"   // STScriptFileList 权威定义（含 ctor/Clear/GetIndex 声明）
STScriptFileList g_stScriptStringFileList;

#include "DNFLexWrapperHelpers/loadRDARScriptFile.inc"


#include "DNFLexWrapperHelpers/unloadRDARScriptFile.inc"


#include "DNFLexWrapperHelpers/ScanType.inc"


#include "DNFLexWrapperHelpers/ScanStrOrInt.inc"


#include "DNFLexWrapperHelpers/ScanTypeOrInt.inc"


#include "DNFLexWrapperHelpers/ScanTypeOrStr.inc"


#include "DNFLexWrapperHelpers/ScanTypeOrFloat.inc"


#include "DNFLexWrapperHelpers/ScanInt.inc"


#include "DNFLexWrapperHelpers/ScanInt_1.inc"


#include "DNFLexWrapperHelpers/ScanFloat.inc"


#include "DNFLexWrapperHelpers/ScanFloat_1.inc"


#include "DNFLexWrapperHelpers/ScanFloat_2.inc"


#include "DNFLexWrapperHelpers/ScanStr.inc"


#include "DNFLexWrapperHelpers/ScanStr_1.inc"


#include "DNFLexWrapperHelpers/ScanFloatStr.inc"


// ORIG B 0x0949b2b8：脚本加载失败回调（LoadStreamFromPlain 失败时以完整路径调用）。
void (*g_fpScriptLoadingErrorFunction)(const char* path) = 0;

#include "DNFLexWrapperHelpers/LoadStreamFromPlain.inc"


#include "DNFLexWrapperHelpers/LoadStreamFromPack.inc"


char* GetEscapeString(MySQL* db, const char* src, char* dst)
{
    // ORIG 0x0861cc47：call MySQL::escape_string（0x083f48aa）后 mov dst,%eax 返回。
    db->escape_string(dst, src);
    return dst;
}

#include "DNFLexWrapperHelpers/getDirectoryPath.inc"


// ============================================================================
// [R3 迁入 DNFLexWrapperHelpers.cpp] ScanAll —— 脚本全类型扫描
// ORIG T 0x088bdea9 / 0x088bde5a，与 ScanStrOrInt(0x088bd0d5)/ScanAnyData(0x088bdb56)
// 同属 ORIG 0x088b 段的 -O0 TU；本 TU 路由为 -O0，天然复现 4.1 -O0 帧/槽。
// token 码：2=dec、3=hex、4=float、5/6=type、7=string、9=pair(int,str)。
// 迁出原因：ScanAll×2 的 optimize("O0") 属性在 -O2 的 DNFLexWrapper.cpp 中触发
// 4.4 TU 级内联抑制，使该 TU 的 _GLOBAL__I__ZN13DNFLexWrapperC2Ev 无法内联
// __ioinit 序列（ORIG 该 GI 为 11 条内联体）。
// ============================================================================

// [b28d-solo-DNFLexWrapper] AE：ORIG 0x088bdea9；token 5/6 拆两个独立 else-if
// （ORIG 两体重复 getType→toTString→assign 序列）⇒ 101=101。
__attribute__((optimize("O0")))
#include "DNFLexWrapperHelpers/ScanAll.inc"


// [b28d-solo-DNFLexWrapper] AE：ORIG 0x088bde5a；帧 0x38、双局部槽 -0x10/-0xc 与
// (float)v 的 -0x1c 转换临时 ⇒ 25=25 逐条同形。
__attribute__((optimize("O0")))
#include "DNFLexWrapperHelpers/ScanAll_1.inc"


// [R3 迁入 DNFLexWrapperHelpers.cpp] g_dnfLex 定义（ORIG B 0x09495020）。
// ORIG 中该对象不在 DNFLexWrapper.cpp TU：其构造在 RDARScript.cpp TU 的
// __static_initialization_and_destruction_0 @0x088c3055（0x088c30ca call
// _ZN13DNFLexWrapperC1Ev，与 PackSet/g_scriptStringManager_ 同序）。
// 定义置本 TU 末尾 ⇒ 不改本 TU GI 名（_GLOBAL__I_g_onLoadScriptFileFunction，
// 首个强外部实体仍是 g_onLoadScriptFileFunction）。
DNFLexWrapper g_dnfLex;
