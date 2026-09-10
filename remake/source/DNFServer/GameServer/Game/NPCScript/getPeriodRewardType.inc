int CNPCScript::getPeriodRewardType() const
{
    // P48/P183：无 name 指针槽，每分支完整成员访问重取（ORIG add $0x674 重算）。
    if (m_npcScript.m_favorableRelationShip.m_durationReward.m_name == "item")
    {
        return 1;
    }
    if (m_npcScript.m_favorableRelationShip.m_durationReward.m_name == "quest")
    {
        return 2;
    }
    return 0;
