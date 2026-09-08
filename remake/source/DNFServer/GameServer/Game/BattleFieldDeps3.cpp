// df_game_r 还原 —— LinkClosureC 批次：CBattle_Field 电梯计时/技能材料统计、
// ::stMapMonsterKillChecker_t 访问器。
// ORIG 依据：
//   CBattle_Field::check_elevator_timer     T 0x0830d902（报告 check_elevator_timer.md）
//   CBattle_Field::GetTotalSkillMaterial    T 0x0830de1c（报告 GetTotalSkillMaterial.md）
//   stMapMonsterKillChecker_t::getUseSkillMaterialCount W 0x0822cb86（objdump：mov 0x14(%eax); ret）
#include "CBattle_Field.h"

#include "CParty.h"
#include "GlobalData.h"
#include "TimerQueue.h"
#include "WarField.h"   // MapInfo / stMapMonsterKillChecker_t（权威布局）

// ORIG 0x0830d902（报告 CBattle_Field/check_elevator_timer.md）：
// ++m_elevatorTimer(+0x139)；若 <4 且 m_elevatorState(+0x138)==2 →
// gen_timer_key(0x28)+GetPartyIndex → InsertTimer(1, partyIdx, msg 0x28, 0xf, key, 0)；
// 若 m_elevatorState==2 → send_elevator_time_packet(0)。
void CBattle_Field::check_elevator_timer()
{
    ++m_elevatorTimer;

    if (m_elevatorTimer < 4 && m_elevatorState == 2)
    {
        // P24/P52：key→esi、partyIdx→ebx 跨 call 缓存，G_TimerQueue 直通
        // receiver——实参内联形（timerKey/partyIndex 无槽）。
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1,
                                    m_party->GetPartyIndex(),
                                    (TIMER_MESSAGE)0x28, 0xf,
                                    m_party->gen_timer_key((TIMER_MESSAGE)0x28), 0);
    }

    if (m_elevatorState == 2)
        send_elevator_time_packet(0);
}

//   → total += m_killChecker1.m_useSkillMaterialTime(+0x4c+0x00)。
//   （ORIG 0830dea6 累加 -0xb4(%ebp)=MapInfo+0x4c=killChecker1 首字段；
//    2026-08-30 Fix21_C 由 m_field14 修正）
int CBattle_Field::GetTotalSkillMaterial()
{
    int total = 0;
    // b28d-lane25：P167 差集实证 ORIG 走 iterator C1Ev + MapInfo C1Ev/D1/aSERKS_
    // ⇒ iterator 与 MapInfo 均在循环外"先声明"，循环内 info = it->second 赋值。
    std::map<int, MapInfo>::iterator it;
    MapInfo info;
    for (it = m_mapInfo.begin(); it != m_mapInfo.end(); ++it)
    {
        info = it->second;
        if (info.m_dummyBossMap != 1 && info.m_visited != 0)
            total += info.m_killChecker1.m_useSkillMaterialTime;
    }
    return total;
}

// ORIG W _ZN25stMapMonsterKillChecker_t22getUseSkillMaterialCountEv @ 0x0822cb86：
// 无独立报告；objdump 逐指令：push ebp / mov 0x14(%eax),%eax / pop ebp / ret
int stMapMonsterKillChecker_t::getUseSkillMaterialCount()
{
    return m_field14;
}
