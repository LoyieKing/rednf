void CParty::CheckHackAverageHitCount()
{
    if (GlobalData::s_villageMonsterMgr->OnEvent())      // 村庄袭击中跳过（0x82f0f44）
        return;
    if (!(m_mapHitCount < 0x2e))                         // >=46 直接返回
        return;

    // 血之副本 / 维度副本 / 样本<=1 → 返回。[b28d-solo-CPartyDepsD2] ORIG
    // 0x085bb0dd..0x085bb12d 为短路链 `A || !B` 寄存器形（无 skip 槽）：
    // A = pDungeon!=0 && (m_89c!=0 || possible>0)，B = m_mapHitCount>1。
    if ((m_battleField.m_pDungeon != 0 &&
         (m_battleField.m_pDungeon->m_89c != 0 ||
          m_battleField.m_pDungeon->get_dimension_possible() > 0)) ||
        !(m_mapHitCount > 1))
        return;

    // 三组平均值（asm fildl/fdivrp：整型转 float 相除）。ORIG 每组的
    // 「float avg=0 预清 → 除法 → 上下界」三件交错（avg1 -0x5c/-0x54/-0x50、
    // avg2 -0x4c/-0x48/-0x44、avg3 -0x3c/-0x38/-0x34），照抄顺序。
    float avg1 = 0.0f;
    avg1 = static_cast<float>(m_mapHitSum) / static_cast<float>(m_mapHitCount);
    int band1Upper = D2_ROUND(avg1 + avg1 * 10.0f + 0.1f);   // fistpl -0x54
    int band1Lower = D2_ROUND(avg1 - avg1 * 10.0f);          // fistpl -0x50
    float avg2 = 0.0f;
    avg2 = static_cast<float>(m_mapHitSum2) / static_cast<float>(m_mapHitCount);
    int band2Upper = D2_ROUND(avg2 + avg2 * 10.0f + 0.1f);
    int band2Lower = D2_ROUND(avg2 - avg2 * 10.0f);
    float avg3 = 0.0f;
    avg3 = static_cast<float>(m_mapHitSum3) / static_cast<float>(m_mapHitCount);
    int band3Upper = D2_ROUND(avg3 + avg3 * 10.0f + 0.1f);
    int band3Lower = D2_ROUND(avg3 - avg3 * 10.0f);

    int inBand1 = 0;                                     // -0x2c（list1 带内）
    int inBand2 = 0;                                     // -0x40（list2 带内）
    int zero2 = 0;                                       // -0x28（list2 为 0）
    int inBand3 = 0;                                     // -0x30（list3 带内）
    int zero3 = 0;                                       // -0x24（list3 为 0）

    for (int i = 0; i < m_mapHitCount; ++i)              // cmp [0x1874] setg
    {
        if (m_mapHitList1[i] <= band1Upper && band1Lower <= m_mapHitList1[i])
            ++inBand1;

        if (m_mapHitList2[i] == 0)
            ++zero2;
        else if (m_mapHitList2[i] <= band2Upper && band2Lower <= m_mapHitList2[i])
            ++inBand2;

        if (m_mapHitList3[i] == 0)
            ++zero3;
        else if (m_mapHitList3[i] <= band3Upper && band3Lower <= m_mapHitList3[i])
            ++inBand3;
    }

    // 第一组：带内数 >= 样本一半 → 0xd9
    if (!(m_mapHitCount / 2 > inBand1))
    {
        for (int i = 0; i < 4; ++i)
            if (_checkValidUser(i))
                memberUser(this, i)->getHackAnalyzer()->addServerHackCnt(
                    memberUser(this, i), (WongWork::ENUM_HACKTYPE)0xd9, 1, 0, 0);
    }

    // 第二组：非零样本中带内过半 → 0xda
    const int nonZero2 = m_mapHitCount - zero2;
    if (nonZero2 > 0 && !(nonZero2 / 2 > inBand2))
    {
        for (int i = 0; i < 4; ++i)
            if (_checkValidUser(i))
                memberUser(this, i)->getHackAnalyzer()->addServerHackCnt(
                    memberUser(this, i), (WongWork::ENUM_HACKTYPE)0xda, 1, 0, 0);
    }

    // 第三组：非零样本中带内过半 → 0xdb
    const int nonZero3 = m_mapHitCount - zero3;
    if (nonZero3 > 0 && !(nonZero3 / 2 > inBand3))
    {
        for (int i = 0; i < 4; ++i)
            if (_checkValidUser(i))
                memberUser(this, i)->getHackAnalyzer()->addServerHackCnt(
                    memberUser(this, i), (WongWork::ENUM_HACKTYPE)0xdb, 1, 0, 0);
    }
