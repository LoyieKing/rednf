// df_game_r 疲劳签到事件 CFatigueAttendance 实现。
// 报告缺失（docs/class_func_reports/CFatigueAttendance 不存在），按 §3.2
// 兜底：nm 定址（C1 0x081173ea / D1 0x08117406 / D0 0x08117436 /
// StartEvent 0x08117454 / StartEvent(Word_Param) 0x08117470 /
// EndEvent 0x081174fe，全部 W 弱符号）+ objdump 区间逐指令转写；
// __PRETTY_FUNCTION__ 字符串与 fmt 字符串取自 .rodata（nm 符号
// CFatigueAttendance::StartEvent(Word_Param)::__PRETTY_FUNCTION__ @0x08b4f000、
// ::EndEvent()::__PRETTY_FUNCTION__ @0x08b4f040、fmt @0x08b4de30/0x08b4de5f）。
#include "CFatigueAttendance.h"

#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x081173ea：仅基类构造 + vptr 置位（无自有成员）。
CFatigueAttendance::CFatigueAttendance()
{
}

// ORIG 0x08117406 / 0x08117436：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
CFatigueAttendance::~CFatigueAttendance()
{
}

// ---------------------------------------------------------------------------
// ORIG 0x08117454：仅 SetEventFlag(true)（无 trace）。
void CFatigueAttendance::StartEvent()
{
    SetEventFlag(true);
}

// ---------------------------------------------------------------------------
// ORIG 0x08117470：param 两 ushort 写入 m_addInfo（+0x04/+0x06）→
// 经 vptr slot2 虚调 StartEvent() →
// cMyTrace(__PRETTY_FUNCTION__,0x1c,0)
// ("CFatigueAttendance(AddInfo:%d,%d) Event Start!", m_info0, m_info1)
// （读取以 movswl 符号扩展，变参 %d 求值序 = ORIG esi/ebx）。
void CFatigueAttendance::StartEvent(Word_Param param)
{
    // N7 封顶：唯一残差=虚调用 callee 寄存器互换（N7，探针 /tmp/l22p/vprobe*.cpp）。
    m_addInfo.m_info0 = param.m_duration;
    m_addInfo.m_info1 = param.m_field2;
    this->StartEvent();
    cMyTrace(__PRETTY_FUNCTION__, 0x1c, 0)(
        "CFatigueAttendance(AddInfo:%d,%d) Event Start!",
        (short)m_addInfo.m_info0, (short)m_addInfo.m_info1);
}

// ---------------------------------------------------------------------------
// ORIG 0x081174fe：SetEventFlag(false) +
// cMyTrace(__PRETTY_FUNCTION__,0x22,0)
// ("CFatigueAttendance Event End!")。
void CFatigueAttendance::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x22, 0);
    tr("CFatigueAttendance Event End!");
}
