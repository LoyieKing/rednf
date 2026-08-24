// df_game_r Game/ CBurningFatigueEvent 实现（批3：从 GameStubs.cpp 迁移）。
// 依据 docs/class_func_reports/CBurningFatigueEvent/*.md + ORIG elf 反汇编：
//   ctor 0x8109fa4：CEventBase() + m_params = G_CDataManager()+0x4f4
//   D1 0x8109ff4：恢复 vptr 后调 CEventBase::~CEventBase()
//   StartEvent 0x810a042：空
//   StartEvent(Word_Param) 0x810a048 / EndEvent 0x810a09a：SetEventFlag + cMyTrace
//   getBonusExpRate 0x810a0ec / getRefillFatigue 0x810a166：遍历 m_params
#include "CBurningFatigueEvent.h"

#include "CGuildServerProxy.h"   // G_CDataManager / CDataManager
#include "cMyTraceNoop.h"

CBurningFatigueEvent::CBurningFatigueEvent()
{
    // ORIG：CEventBase ctor 置基类 vptr 与 stADDINFO/+0x08 flag；
    // m_params 指向全局 CDataManager 内 +0x4f4 的 burning-fatigue 参数 vector。
    m_params = reinterpret_cast<std::vector<stBurningFatigueParam_t>*>(
        reinterpret_cast<char*>(G_CDataManager()) + 0x4f4);
}

CBurningFatigueEvent::~CBurningFatigueEvent()
{
    // ORIG D1：恢复本类 vptr，再调 CEventBase::~CEventBase()（空）。
}

void CBurningFatigueEvent::StartEvent()
{
    // ORIG 0x810a042：空实现。
}

void CBurningFatigueEvent::StartEvent(Word_Param /*param*/)
{
    // ORIG 0x810a048：SetEventFlag(true) + 日志。
    SetEventFlag(true);
    cMyTrace tr("virtual void CBurningFatigueEvent::StartEvent(Word_Param)", 0x1b, 0);
    tr("Burning Fatigue Event Start");
}

void CBurningFatigueEvent::EndEvent()
{
    // ORIG 0x810a09a：SetEventFlag(false) + 日志。
    SetEventFlag(false);
    cMyTrace tr("virtual void CBurningFatigueEvent::EndEvent()", 0x21, 0);
    tr("Burning Fatigue Event End");
}

int CBurningFatigueEvent::getBonusExpRate(unsigned short level) const
{
    // ORIG 0x810a0ec：从 i=1 开始找第一个 m_level > level 的项，返回其前一项
    // m_bonusExpRate(+0x04)；越界返回 0。
    const std::vector<stBurningFatigueParam_t>& v = *m_params;
    for (size_t i = 1; i < v.size(); ++i)
    {
        if (level < v[i].m_level)
            return v[i - 1].m_bonusExpRate;
    }
    return 0;
}

int CBurningFatigueEvent::getRefillFatigue(unsigned short level,
                                           unsigned short cur) const
{
    // ORIG 0x810a166：找第一个满足 (level >= m_level) 且 m_level > (level-cur)
    // 的项，返回其 m_refillFatigue(+0x08)；越界返回 0。
    const std::vector<stBurningFatigueParam_t>& v = *m_params;
    for (size_t i = 1; i < v.size(); ++i)
    {
        if (level < v[i].m_level)
            continue;
        if ((int)(unsigned int)v[i].m_level <= (int)((unsigned int)level - (unsigned int)cur))
            continue;
        return v[i].m_refillFatigue;
    }
    return 0;
}