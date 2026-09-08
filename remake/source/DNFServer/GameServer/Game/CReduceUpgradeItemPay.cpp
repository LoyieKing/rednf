// df_game_r 强化减费事件 CReduceUpgradeItemPay 实现。
// 报告缺失（§3.2 兜底）：nm 定址 + objdump 逐指令转写
// （0x0811715c..0x081172c1；GetReduceRate 0x0854b56e）。
#include "CReduceUpgradeItemPay.h"

#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x0811715c：仅基类构造 + vptr 置位（无自有成员初始化）。
CReduceUpgradeItemPay::CReduceUpgradeItemPay()
{
}

// ORIG 0x08117178 / 0x081171a8：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
CReduceUpgradeItemPay::~CReduceUpgradeItemPay()
{
}

// ---------------------------------------------------------------------------
// ORIG 0x081171c6：仅 SetEventFlag(true)，无 trace（sub $0x18 帧实证）。
void CReduceUpgradeItemPay::StartEvent()
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
void CReduceUpgradeItemPay::StartEvent(Word_Param param)
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
void CReduceUpgradeItemPay::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x22, 0);
    tr("CReduceUpgradeItemPay Event End!");
}

// ---------------------------------------------------------------------------
// ORIG W 0x0854b56e：movzwl 0x4(%eax),%eax; cwtl; ret —— +0x04 ushort 经
// 符号扩展返回 int（P11：movzwl 读 + cwtl 符号扩展 ⇔ (short) cast）。
int CReduceUpgradeItemPay::GetReduceRate()
{
    return (short)m_addInfo.m_info0;
}
