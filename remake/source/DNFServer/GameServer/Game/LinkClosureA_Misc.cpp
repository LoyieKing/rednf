// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 LinkClosureA_Misc/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <stdio.h>
#include <time.h>

#include "CItem.h"            // IsEnableWorld 两重载 / m_field68
#include "MatchRoomList.h"    // IMatch::GetUser 虚槽 +0x10
#include "CUser.h"            // get_acc_name
#include "advancealtar.h"

// [L32 GI 调序] ORIG `_GLOBAL__I__ZN12advancealtar7Manager12gmTimeSpeed_E`
//   （CU=global_AdvanceAltar.cpp）⇒ 本 TU 首个强符号须为 advancealtar::Manager::gmTimeSpeed_。
//   仅把该定义提到文件首（数值初值 1 为静态初始化，无构造序影响；P327）。
namespace advancealtar
{

// gmTimeSpeed_ D 符号定义（ORIG .data 初值 1 实证）。
int Manager::gmTimeSpeed_ = 1;

}  // namespace advancealtar

#include "LinkClosureA_Misc/getAdvanceAltarNeedGage.inc"


#include "LinkClosureA_Misc/EnableWorld.inc"


// ---- GetUserName（ORIG T 0x08563ff7 报告缺失，0x159 字节 objdump 转写）---
// 对 team 3/2/1/0 各两次调用 match->GetUser(team)（虚槽+0x10，P48 双 call），
// 命中取 user->get_acc_name()，否则回退空串（ORIG 四个 false 臂同用
// 0x8ca21e0 = ""，紧邻的 "127.0.0.1"@0x8ca21e1 属别处常量，objdump -s 实证）；
// 最后 sprintf(out, "%s,%s,%s,%s", t0, t1, t2, t3) 且返回 out。
// [b28d-7 N7/N82] ORIG 0x08563ff7：间接 call 目标 %edx vs %eax（N7，446h46 下仍未翻）⇒ 封顶。
// [LaneA 落地 · P0/P183 删具名局部] ORIG 0x08563ff7 四个回退值（false 臂 `mov $"",%reg`）
// 全程落 callee-saved 寄存器 edi/esi/ebx/eax、**无槽往返** ⇒ 原码无具名局部：四条
// `?:` 表达式内联在 sprintf 实参位。发射序 team3→team2→team1→team0 正是 cdecl
// 实参右→左求值序（末实参 n3 先算 ⇒ 跨最多调用保活 ⇒ 占 edi），与 ORIG 逐块一致。
// 旧形（4 个具名局部）多 4 槽 + 帧 0x38（ORIG push edi/esi/ebx + 帧 0x2c）。
char* GetUserName(IMatch* match, char* out)
{
    sprintf(out, "%s,%s,%s,%s",
            match->GetUser(0) != 0 ? match->GetUser(0)->get_acc_name() : "",
            match->GetUser(1) != 0 ? match->GetUser(1)->get_acc_name() : "",
            match->GetUser(2) != 0 ? match->GetUser(2)->get_acc_name() : "",
            match->GetUser(3) != 0 ? match->GetUser(3)->get_acc_name() : "");
    // [0x8564132 fmt @0x8ca23b5 "%s,%s,%s,%s"；空串 @0x8ca21e0]
    return out;
}

// ---- GetCurrentResetBaseTime（报告全文已读，见头注路径）-------------------
// ORIG：t2 = t-0x15180；localtime_r(&t) 与 localtime_r(&t2) 双视图；按当前
// 小时与入参比较把命中视图的时/分/秒改写后 mktime 规范化（无返回值写出）。
// 定义由 DNFFunctionLib.cpp 提供（void GetCurrentResetBaseTime(long,int)，
// mangled _Z23GetCurrentResetBaseTimeli 与 ORIG 一致）；本 TU 不再重复定义，
// 避免生成 ORIG 不存在的 _Z23GetCurrentResetBaseTimeii 冗余符号。

// [L32 GI 调序] advancealtar::Manager::gmTimeSpeed_ 的定义已提到文件首（见上），
// 原尾部的重复定义已删（同一符号仅一处定义点）。
