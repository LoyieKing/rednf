// ============================================================================
// ClearDungeonMission::rewardMission —— ORIG T 0x08165484（rewardMission.md 全文）。
// 判活顺序：已奖励（非重复且 fieldA!=0）→ 任务条件不满足（type==2 且
// field8!=7）→ 计数未达上限 → rewardProcess 发奖并 fieldA++/field6=1，
// 重复任务 reset。
// ============================================================================
bool ClearDungeonMission::rewardMission(CUser* user, HeroMissionValue& value)
{
    if (!isRepeatMission() && value.m_fieldA != 0)
    {
        LogManager::logFormat(
            1, "localtaiwan/Event/EventHeroMission.cpp",
            __PRETTY_FUNCTION__,
            0xac,
            "[HeroMission] Already reward. (charac:%u,mission:%d,count:%d)",
            user->get_charac_no(-1), value.m_missionType, value.m_fieldA);
        return false;
    }
    if (getMissionType() == 2 && value.m_field8 != 7)
    {
        LogManager::logFormat(
            1, "localtaiwan/Event/EventHeroMission.cpp",
            __PRETTY_FUNCTION__,
            0xb4,
            "[HeroMission] Invalid reward condition. (charac:%u,mission:%d,value:%d)",
            user->get_charac_no(-1), value.m_missionType, value.m_field8);
        return false;
    }
    if (value.m_field4 < m_condition.m_extra[0])
    {
        LogManager::logFormat(
            1, "localtaiwan/Event/EventHeroMission.cpp",
            __PRETTY_FUNCTION__,
            0xbb,
            "[HeroMission] Invalid reward condition. (charac:%u,mission:%d,count:%d)",
            user->get_charac_no(-1), value.m_missionType, value.m_fieldA);
        return false;
    }
    bool ok = rewardProcess(user);
    value.m_fieldA = value.m_fieldA + 1;
    value.m_field6 = 1;
    if (isRepeatMission())
    {
        value.reset();
    }
    return ok;
}