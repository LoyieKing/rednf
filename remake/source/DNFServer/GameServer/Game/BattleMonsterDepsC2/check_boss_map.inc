// ORIG 0x083079ba：终点位 boss 房条件未满足时报错码 7。
// [b28d-solo-BattleMonsterDepsC2] P13/P283：ORIG begin/end 各转换构造一次后 `jmp` 底部条件块
// （it=-0x14、itEnd=-0x18 两独立槽）⇒ 源写「两个具名 const_iterator + for(;it!=itEnd;++it)」；
// 条件里直接调 end() 使 4.4 复用 -0x14 槽并每轮重调 end()（-5 条）。
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
}