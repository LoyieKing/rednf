// ORIG 0x85e2e74：越界时 cMyTrace 记录 InvalidRank 并返回 0。
unsigned int PvP_MissionSystem::get_BaseMissionExp_byRank(unsigned short rank) const
{
    if (rank >= m_baseMissionExp.size())
    {
        // [P2Free304to323 2026-09-11 落地] 返回类型负结论已被 Q6HeaderTypeAxis 的头轴
        //   修复推翻：`CDataManager.h` 声明改 `unsigned int`（ORIG prett 直证
        //   @0x8cc3b00 size **0x54**），本定义行同步 ⇒ `__PRETTY_FUNCTION__` 现发射
        //   `_ZZNK17PvP_MissionSystem25get_BaseMissionExp_byRankEtE19__PRETTY_FUNCTION__`
        //   且串逐字节等于 ORIG（回收 P139 缺口与 missing_in_game）。
        cMyTrace(__PRETTY_FUNCTION__, 0x162, 5)
            ("[@missionSystem] <InvalidRank : %d>", rank);
        return 0;
    }
    // ORIG 0x085e2ef8：`mov (%eax),%eax` 直传（成员为 vector<unsigned int>，
    // 返回类型 unsigned int 已对齐 ⇒ 无需 cast）。
    return m_baseMissionExp[rank];
}