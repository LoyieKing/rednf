// ============================================================================
// df_game_r 还原 —— HeroMissionEvent（typo 批次，替换 GameStubs 桥接）。
// processMission @ 0x081672be / findMissionValue @ 0x08168010
// 忠实转写 docs/class_func_reports/HeroMissionEvent/{processMission,findMissionValue}.md。
// 判活：ORIG 经 vptr+0x34 调用 CEventBase::IsEventing(NULL)（即读取 +0x08 标志）。
// ============================================================================

#include "HeroMissionEvent.h"

#include "CUser.h"
#include "LogManager.h"

// ---- 显式实例化：ARAD::Singleton<HeroMissionEvent>::Get()（ORIG W 0x0811c9ce）----
namespace ARAD
{
template HeroMissionEvent* Singleton<HeroMissionEvent>::Get();
}

HeroMissionEvent::HeroMissionEvent()
{
    // CEventBase::CEventBase()（0x0810ae2c）：stADDINFO ctor 置 0xffff，flag 置 0
    m_addInfo0 = 0xffff;
    m_addInfo1 = 0xffff;
    m_isEventing = 0;
    // m_conditions / m_values 默认构造（ORIG ctor 0x08166552 / 0x08166560）
}

// ============================================================================
// findMissionValue @ 0x08168010
//   bool findMissionValue(CUser*, MissionNo::T, HeroMissionValue*&)
//   判活 → mission ∈ [1,6] → m_values[charac_no] 的 vector 中匹配 mission。
// ============================================================================
bool HeroMissionEvent::findMissionValue(CUser* user,
                                        HeroMissionCondition::MissionNo::T mission,
                                        HeroMissionValue*& out)
{
    if (!m_isEventing)
        return false;
    if (mission <= 0 || mission >= 7)
        return false;

    unsigned int characNo =
        static_cast<unsigned int>(user->get_charac_no(-1));
    std::map<unsigned int, std::vector<HeroMissionValue> >::iterator it =
        m_values.find(characNo);
    if (it == m_values.end())
        return false;

    for (std::vector<HeroMissionValue>::iterator vit = it->second.begin();
         vit != it->second.end(); ++vit)
    {
        if (vit->m_mission == static_cast<int>(mission))
        {
            out = &*vit;
            return true;
        }
    }
    return false;
}

// ============================================================================
// processMission @ 0x081672be
//   判活 → user 判空 → findMissionValue → m_conditions[mission] 条件对象
//   处理（虚表 +0x08）→ mission==4 且进度刚置位则递归触发 mission 6。
// ============================================================================
void HeroMissionEvent::processMission(CUser* user,
                                      HeroMissionCondition::MissionNo::T mission,
                                      unsigned int param)
{
    if (!m_isEventing)
        return;

    if (user == 0)
    {
        LogManager::logFormat(1, "localtaiwan/Event/EventHeroMission.cpp",
                              __PRETTY_FUNCTION__, 0x2b8,
                              "[HeroMission] User is null.");
        return;
    }

    HeroMissionValue* value = 0;
    if (!findMissionValue(user, mission, value))
        return;

    std::map<HeroMissionCondition::MissionNo::T,
             BaseHeroMissionCondition*>::iterator it =
        m_conditions.find(mission);
    if (it == m_conditions.end())
    {
        LogManager::logFormat(1, "localtaiwan/Event/EventHeroMission.cpp",
                              __PRETTY_FUNCTION__, 0x2c2,
                              "[HeroMission] Invalid mission. (charac:%u, mission:%d)",
                              user->get_charac_no(-1),
                              static_cast<int>(mission));
        user->DisConnSig(static_cast<DISCONN_SIG>(0x3e), 1, 0x2c3);
        return;
    }

    unsigned short prev = value->m_fieldA;
    it->second->process(user, value, param);   // 条件虚表 +0x08
    if (mission == 4 && prev == 0 && value->m_fieldA != 0)
    {
        processMission(user, static_cast<HeroMissionCondition::MissionNo::T>(6), 1);
    }
}
