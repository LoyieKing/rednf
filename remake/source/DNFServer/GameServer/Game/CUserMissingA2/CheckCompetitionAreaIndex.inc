bool CHARAC_LOAD_MERCENARY::CheckCompetitionAreaIndex(char areaIdx,
                                                      int level) const
{
    // ORIG：G_CDataManager() 在 empty()/size()/operator[] 三处各自重调（禁缓存，
    // P183/P48）；经非 const CDataManager* 直访 m_vec40 选非 const operator[]。
    // ORIG 0x081b2b53：`call empty; test %al,%al; jne L_true`（**无 xor**）+
    // 0x081b2b69 `cmp %edx,%eax; jae L_false` ⇒ 原码是正极性短路
    // `if (empty() || size < (unsigned)areaIdx) return 0;`（原实现写
    // !empty && … 出 xor$1 物化 + ja，块序全反）。bool 具名局部会额外占槽
    // （4.4 出 movb/cmpb 对，帧 0x34 vs ORIG 0x24）⇒ 必须无局部直写 if。
    if (G_CDataManager()->m_mercenarySystem.m_vec40.empty() ||
        G_CDataManager()->m_mercenarySystem.m_vec40.size() <
            static_cast<unsigned int>(static_cast<int>(areaIdx)))
        return false;
    if (level < static_cast<int>(static_cast<short>(
            G_CDataManager()->m_mercenarySystem
                .m_vec40[static_cast<int>(areaIdx)].m_areaLevel)))
        return false;
    return true;
