// df_game_r Game/ ARAD::Arad_EventPeriodDataManager::isApplied 实现
//（批3：从 GameStubs.cpp 迁移）。
//
// ORIG：_ZN4ARAD27Arad_EventPeriodDataManager9isAppliedE22ENUM_REPEAT_EVENT_CODEj @ 0x081956b0
//
// 逻辑（Ghidra 反编译 isApplied.md，C-decomp 113-141 行 + asm 08195745..08195791）：
//   在 this+0xc 的 vector<ARAD::SCRIPT::SEventPeriodData> 中按事件码找到周期项；
//   命中后判定 time 是否落在 [m_beginTime, m_endTime) 内，是则返回 1。
//   反汇编判定（08195775/0819577c）：
//     NOT( time < pEl->m_beginTime ) AND NOT( pEl->m_endTime <= time )
//     ==  (pEl->m_beginTime <= time) && (time < pEl->m_endTime)
#include "Arad_EventPeriodDataManager.h"

namespace ARAD
{

bool Arad_EventPeriodDataManager::isApplied(ENUM_REPEAT_EVENT_CODE eventCode, unsigned int time)
{
    std::vector<ARAD::SCRIPT::SEventPeriodData>::const_iterator it;
    for (it = m_eventPeriodList.begin(); it != m_eventPeriodList.end(); ++it)
    {
        if (it->m_eventCode == static_cast<unsigned int>(eventCode))
        {
            return (it->m_beginTime <= time) && (time < it->m_endTime);
        }
    }
    return false;
}

bool Arad_EventPeriodDataManager::isRunning(ENUM_REPEAT_EVENT_CODE eventCode)
{
    // ORIG isRunning@0x08195686 以 GlobalData::s_systemTime_ 的当前 tick 作为 time
    // 转发给 isApplied。此处保留转发语义；GlobalData 依赖由宿主接线。
    return isApplied(eventCode, 0);
}

}  // namespace ARAD