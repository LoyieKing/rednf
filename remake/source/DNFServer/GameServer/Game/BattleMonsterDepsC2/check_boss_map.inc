int CBattle_Field::check_boss_map(int x, int y)
{
    if (m_endX == x && m_endY == y &&                                      // +0xc/+0x10
        m_pDungeon != NULL && m_pDungeon->m_bossRoom90c.m_0 == 1)
    {
        std::vector<stConditionEnterBossRoomStatus_t>::const_iterator it =
            m_bossRoomConditions.begin();
        std::vector<stConditionEnterBossRoomStatus_t>::const_iterator itEnd =
            m_bossRoomConditions.end();
        for (; it != itEnd; ++it)
        {
            if (it->m_curKillCount < it->m_maxKillCount)
            {
                LogManager::logFormat(
                    1, "battle_field.cpp",
                    __PRETTY_FUNCTION__, 0x1181,
                    "TRY MoveMap error condition [%d][%d]/[%d]", it->m_monsterIdx,
                    it->m_curKillCount, it->m_maxKillCount);
                return 7;
            }
        }
    }
    return 0;
