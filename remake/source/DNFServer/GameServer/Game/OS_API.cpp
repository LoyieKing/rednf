// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 OS_API/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include <cstdio>
#include <cstdlib>
#include <cstdarg>

#include "CMyFileLog.h"
char* NumberToString(unsigned int value, int radix);   // 定义 LinkFinalDeps.cpp/ServerCommon

// 5 个 ORIG 符号的实现（df_game_r 0x0858c53a..0x0858c789）。
// 其余 5 符号（GetDateTimeTick/snprintf/Sleep/WriteLog/uSleep）由既有文件以
// namespace OS_API 定义，声明见 OS_API.h 与 CInventory.h。
// [R11fixE7 2026-09-12] OS_API 在 ORIG 中是 **class（全 static 成员）**：ORIG prett
// `static void OS_API::WriteLastSig(const char*, const char*, memberIdentificationNumber_t,
// int, int, const void*, int, int, int, int, int, int)`（raw @0x8cad3c0）**含 `static `
// 前缀**且 ORIG nm 该符号为 T（0x0858c5f4）——namespace 自由函数无法同时满足
// 「prett 有 static」（内部链接 t）与「符号 T」。故本 TU 的定义处改 class 静态成员
// 形式（本文件不再 include OS_API.h，避免同名 namespace/class 冲突）；其余消费 TU 的
// `namespace OS_API { ... }` 最小声明保持不变 —— mangled/调用约定（T/cdecl，无 this）/
// 链接性均不变。
typedef unsigned int memberIdentificationNumber_t;

class OS_API
{
public:
    static void Exit(int exitCode);
    static void WriteLastBookmark(char const* path, int value);
    static void WriteLastSig(char const* dir, char const* name,
                             int k, int c, void const* p, int sz);
    static void WriteLastSig(char const* dir, char const* name,
                             memberIdentificationNumber_t u, int k, int c,
                             void const* p, int tq, int dq, int nq, int dbq,
                             int logdbq, int extra);
    static void WriteLoginInfo(char const* path, char const* fmt, ...);
};

#include "OS_API/Exit.inc"


#include "OS_API/WriteLastBookmark.inc"


#include "OS_API/WriteLastSig.inc"


#include "OS_API/WriteLastSig_1.inc"


#include "OS_API/WriteLoginInfo.inc"

