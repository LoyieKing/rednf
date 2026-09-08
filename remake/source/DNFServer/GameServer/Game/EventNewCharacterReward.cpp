// df_game_r 还原 —— EventNewCharacterReward 事件实体（新手角色活动奖励）。
// 全部函数逐指令对照 docs/class_func_reports/EventNewCharacterReward/*.md
// 的 ## 汇编 与 ## 反编译 C 转写（报告地址见各函数头注释）：
//   C1 0x0816bac4 / D1 0x0816baf4 / D0 0x0816bb24 / StartEvent() 0x0816bb42 /
//   StartEvent(Word_Param) 0x0816bb94 / EndEvent() 0x0816bc98 /
//   isEventCharacter 0x0816bcea / eventExpRatio 0x0816bd06 /
//   eventCharacterJob 0x0816bd22
#include "EventNewCharacterReward.h"
#include "GameTypes.h"   // cMyTrace

// ORIG C1 0x0816bac4：CEventBase ctor + vptr 设置，+0x0c m_job=-1、
// +0x10 m_field10=0（每字段重读 this，函数体赋值形态）。
EventNewCharacterReward::EventNewCharacterReward()
{
    m_job = -1;
    m_field10 = 0;
}

// ORIG D1 0x0816baf4：vptr 设置 + CEventBase::~CEventBase() + 删除标志检查。
EventNewCharacterReward::~EventNewCharacterReward()
{
}

// ORIG 0x0816bb42：SetEventFlag(true) + cMyTrace(__PRETTY_FUNCTION__,0x16,0)。
void EventNewCharacterReward::StartEvent()
{
    SetEventFlag(true);
    cMyTrace trace(__PRETTY_FUNCTION__, 0x16, 0);
    trace("[Taiwan, EventCharacterBonus] Event start!");
}

// ORIG 0x0816bb94：param 两个 ushort → +0x0c/+0x10，并同步 m_addInfo；
// m_job∈[0,10] 时日志(0x22) + 虚调 StartEvent()（vptr+8），否则日志(0x27)。
// [b28c-31] N7：末尾虚调用 callee ORIG %edx vs 4.4 %eax（76=76；其余仅字符串
// 立即数/call 目标重定位显示差）。
void EventNewCharacterReward::StartEvent(Word_Param param)
{
    m_job = param.m_duration;
    m_field10 = param.m_field2;
    m_addInfo.m_info0 = static_cast<unsigned short>(m_job);
    m_addInfo.m_info1 = static_cast<unsigned short>(m_field10);
    if (m_job >= 0 && m_job <= 10)
    {
        // P37 临时链式：args 表达式分派到 callee-saved 寄存器（ORIG esi/ebx）。
        cMyTrace(__PRETTY_FUNCTION__, 0x22, 0)(
            "[Taiwan, EventCharacterBonus] Event parameters (job:%d, exp_ratio:%d)",
            m_job, m_field10);
        this->StartEvent();
    }
    else
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x27, 0)(
            "[Taiwan, EventCharacterBonus] Invalid event parameters (job:%d, exp_ratio:%d)",
            m_job, m_field10);
    }
}

// ORIG 0x0816bc98：SetEventFlag(false) + cMyTrace(__PRETTY_FUNCTION__,0x2f,0)。
void EventNewCharacterReward::EndEvent()
{
    SetEventFlag(false);
    cMyTrace trace(__PRETTY_FUNCTION__, 0x2f, 0);
    trace("[Taiwan, EventCharacterBonus] Event stop!");
}

// ORIG 0x0816bcea（size 0x1c）：+0x0c == param_1（分支形态 cmp/jne+mov 1/0）。
bool EventNewCharacterReward::isEventCharacter(int job)
{
    if (m_job == job)
        return true;
    return false;
}

// ORIG 0x0816bd06：fildl +0x10 千分数 / 100.0f（_DAT_08b80014=0x42c80000）。
float EventNewCharacterReward::eventExpRatio()
{
    return (float)m_field10 / 100.0f;
}

// ORIG 0x0816bd22：直返 +0x0c。
int EventNewCharacterReward::eventCharacterJob()
{
    return m_job;
}
