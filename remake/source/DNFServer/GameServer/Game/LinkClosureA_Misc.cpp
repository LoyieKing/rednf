// ============================================================================
// LinkClosureA_Misc.cpp — undef_bizA 收口批 A 杂项域
//
// 覆盖符号（唯一 owner = 本 TU）：
//   CItem::getAdvanceAltarNeedGage        W 0x081347ca（报告缺失，objdump 转写）
//   ::EnableWorld(CItem const*, CItem const*) T 0x0849d768（报告缺失）
//   GetUserName(IMatch*, char*)           T 0x08563ff7（报告缺失）
//   GetCurrentResetBaseTime(long, int)    T 0x0810912a
//     （报告 docs/class_func_reports/_lt_global_gt_/GetCurrentResetBaseTime.md）
//   advancealtar::Manager::gmTimeSpeed_   D 0x09370094（4 字节 .data 初值 1，
//     ctor 记录 _GLOBAL__I..gmTimeSpeed_E 0x081343f9 静态初始化实证）
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

// ---- CItem::getAdvanceAltarNeedGage（ORIG W 0x081347ca 报告缺失）----------
// ORIG 汇编（objdump 0x81347cd）：push %ebp; mov %esp,%ebp;
// mov 0x8(%ebp),%eax; mov 0x68(%eax),%eax; pop %ebp; ret —— 直返 +0x68 字段。
int CItem::getAdvanceAltarNeedGage() const
{
    return m_field68;
}

// ---- EnableWorld（ORIG T 0x0849d768 报告缺失，77 字节 objdump 全文转写）--
// 双向校验（ORIG 0x849d76e 段 this=0x8(%ebp)=a、arg=0xc(%ebp)=b；
// 0x849d78e 段 this=0xc(%ebp)=b、arg=0x8(%ebp)=a——objdump 实参槽实证）：
// 任一方向失败返 0，否则返 1。
bool EnableWorld(const CItem* a, const CItem* b)
{
    if (!a->IsEnableWorld(b))           // [objdump 0x849d76e..0x787]
    {
        return false;                   // [0x849d780 xor $1; 失败 mov $0]
    }
    if (!b->IsEnableWorld(a))           // [0x849d78e this=b, arg=a]
    {
        return false;                   // [0x849d79b..0x7a7]
    }
    return true;                        // [0x849d7ae mov $1]
}

// ---- GetUserName（ORIG T 0x08563ff7 报告缺失，0x159 字节 objdump 转写）---
// 对 team 3/2/1/0 各两次调用 match->GetUser(team)（虚槽+0x10，P48 双 call），
// 命中取 user->get_acc_name()，否则回退 "127.0.0.1"；最后
// sprintf(out, "%s,%s,%s,%s", t0, t1, t2, t3) 且返回 out。
// [b28d-7 N7/N82] ORIG 0x08563ff7：push %edi/%esi/%ebx + 帧 $0x2c，4.4 无 push + 帧 $0x38；间接 call 目标 %edx vs %eax（N7，446h46 下仍未翻）⇒ 封顶。
char* GetUserName(IMatch* match, char* out)
{
    const char* n3 = match->GetUser(3) != 0
                         ? match->GetUser(3)->get_acc_name()
                         : "";                     // [edi 槽]
    const char* n2 = match->GetUser(2) != 0
                         ? match->GetUser(2)->get_acc_name()
                         : "";                     // [esi 槽]
    const char* n1 = match->GetUser(1) != 0
                         ? match->GetUser(1)->get_acc_name()
                         : "";                     // [ebx 槽]
    const char* n0 = match->GetUser(0) != 0
                         ? match->GetUser(0)->get_acc_name()
                         : "";                     // [eax 槽]
    sprintf(out, "%s,%s,%s,%s", n0, n1, n2, n3);
                                                            // [0x8564132 fmt @0x8ca23b5]
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
