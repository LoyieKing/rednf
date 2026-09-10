// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CReduceUpgradeItemPay/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 强化减费事件 CReduceUpgradeItemPay 实现。
// 报告缺失（§3.2 兜底）：nm 定址 + objdump 逐指令转写
// （0x0811715c..0x081172c1；GetReduceRate 0x0854b56e）。
#include "CReduceUpgradeItemPay.h"

#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x0811715c：仅基类构造 + vptr 置位（无自有成员初始化）。
#include "CReduceUpgradeItemPay/CReduceUpgradeItemPay.inc"
{
}

// ORIG 0x08117178 / 0x081171a8：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
#include "CReduceUpgradeItemPay/dtor_CReduceUpgradeItemPay.inc"
{
}

// ---------------------------------------------------------------------------
// ORIG 0x081171c6：仅 SetEventFlag(true)，无 trace（sub $0x18 帧实证）。
#include "CReduceUpgradeItemPay/StartEvent.inc"
{
    SetEventFlag(true);
}

// ---------------------------------------------------------------------------
// ORIG 0x081171e2：param 两 ushort 写入基类 stADDINFO（+0x04/+0x06：
// movzwl 0xc(%ebp) → mov %dx,0x4(%eax)；movzwl 0xe(%ebp) → mov %dx,0x6(%eax)）
// → 虚调 StartEvent() → trace 行 0x1c，变参为两字段经 cwtl 符号扩展的
// (short) 值（movswl %ax,%esi/ebx；压栈序 0xc=info1、0x8=info0）。
// [b28c-30] N7 封顶：虚调段 ORIG 间接目标 %edx vs 4.4 恒 %eax（同族三件）；
// 其余行级差为 cMyTrace 字符串/函数地址的 .o 重定位伪差（AE 口径已归一）。
#include "CReduceUpgradeItemPay/StartEvent_1.inc"
{
    m_addInfo.m_info0 = param.m_duration;
    m_addInfo.m_info1 = param.m_field2;
    StartEvent();
    cMyTrace(__PRETTY_FUNCTION__, 0x1c, 0)
        ("CReduceUpgradeItemPay(AddInfo:%d,%d) Event Start!",
         (short)m_addInfo.m_info0, (short)m_addInfo.m_info1);
}

// ---------------------------------------------------------------------------
// ORIG 0x08117270：SetEventFlag(false) +
// cMyTrace(__PRETTY_FUNCTION__,0x22,0)
// ("CReduceUpgradeItemPay Event End!")。
#include "CReduceUpgradeItemPay/EndEvent.inc"
{
    SetEventFlag(false);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x22, 0);
    tr("CReduceUpgradeItemPay Event End!");
}

// ---------------------------------------------------------------------------
// ORIG W 0x0854b56e：movzwl 0x4(%eax),%eax; cwtl; ret —— +0x04 ushort 经
// 符号扩展返回 int（P11：movzwl 读 + cwtl 符号扩展 ⇔ (short) cast）。
#include "CReduceUpgradeItemPay/GetReduceRate.inc"
{
    return (short)m_addInfo.m_info0;
}
