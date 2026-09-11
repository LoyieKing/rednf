// ---------------------------------------------------------------------------
// CheckHackAverageHitCount —— ORIG T 0x085bb0ac
// 报告：docs/class_func_reports/CParty/CheckHackAverageHitCount.md
// 常量实证（objdump rodata）：0x08cbbbfc=10.0f（乘）、0x08cbbc00=0.1f（加）。
// 三组命中样本带外统计过半 → 全体有效成员 addServerHackCnt 0xd9/0xda/0xdb。
// ---------------------------------------------------------------------------
// [b28d-solo-CPartyDepsD2] 判词 N82/N95（361=361）：已修 ①skip 改 ORIG 短路链
// 寄存器形 `A || !B`（A=pDungeon!=0&&(m_89c!=0||possible>0)，消 skip 槽）
// ②avg/band 三组交错 + float 预清两句式（ORIG -0x5c/-0x54/-0x50 族）。
// [b28d-solo-CPartyDepsD2²] DIFF→AE 归零（361→361，AE 361/361）：三旋钮联动——
// ①P252 ORIG 0x085bb163 `flds avg; flds 10.0f; fmulp; fstps -0x58` +
//   0x085bb171 `flds avg; fadds -0x58` ⇒ 原码有具名 float 临时（三组复用同一
//   槽、无预清）⇒ 补 `float bandTmp;`；
// ②P212 全头部声明：4.4 块内「先声明浅」与 4.1 相反，声明整体提至函数头后
//   同为「先声明深」单调 ⇒ 声明序按 ORIG 槽深→浅（avg1 -0x5c/tmp -0x58/…）
//   使 0x94 帧与全部槽号（含 -0x40/-0x30/-0x2c 计数器区）逐位对齐；
// ③0x085bb111 `test %al,%al; jg` 为**有符号**比较 ⇒ `(signed char)
//   get_dimension_possible() > 0`（uchar 的 `> 0` 恒折 `jne`）。
// 残余：0（check_tu_routed 已判 AE）。
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
          (signed char)m_battleField.m_pDungeon->get_dimension_possible() > 0)) ||
        !(m_mapHitCount > 1))
        return;

    // 三组平均值（asm fildl/fdivrp：整型转 float 相除）。ORIG 每组的
    // 「float avg=0 预清 → 除法 → 上下界」三件交错（avg1 -0x5c/-0x54/-0x50、
    // avg2 -0x4c/-0x48/-0x44、avg3 -0x3c/-0x38/-0x34），照抄顺序。
    // [b28d-solo-CPartyDepsD2²] P212 全头部声明（4.4 块内作用域「先声明浅」与
    // 4.1 相反；声明整体提至函数头后同为「先声明深」单调）：声明序按 ORIG 槽
    // 深→浅排（avg1 -0x5c / tmp -0x58 / band1U -0x54 …zero3 -0x24），初始化
    // 语句留在原处（int 清零与 float 预清发射序照 ORIG）。P252：ORIG
    // 0x085bb163 `flds avg; flds 10.0f; fmulp; fstps -0x58` + `flds avg;
    // fadds -0x58` ⇒ 原码有具名 float 临时（三组复用同一槽，且无预清）。
    float avg1;          // -0x5c
    float bandTmp;       // -0x58（三组复用）
    int band1Upper;      // -0x54
    int band1Lower;      // -0x50
    float avg2;          // -0x4c
    int band2Upper;      // -0x48
    int band2Lower;      // -0x44
    int inBand2;         // -0x40
    float avg3;          // -0x3c
    int band3Upper;      // -0x38
    int band3Lower;      // -0x34
    int inBand3;         // -0x30
    int inBand1;         // -0x2c
    int zero2;           // -0x28
    int zero3;           // -0x24

    avg1 = 0.0f;
    avg1 = static_cast<float>(m_mapHitSum) / static_cast<float>(m_mapHitCount);
    bandTmp = avg1 * 10.0f;
    band1Upper = D2_ROUND(avg1 + bandTmp + 0.1f);            // fistpl -0x54
    band1Lower = D2_ROUND(avg1 - bandTmp);                   // fistpl -0x50
    avg2 = 0.0f;
    avg2 = static_cast<float>(m_mapHitSum2) / static_cast<float>(m_mapHitCount);
    bandTmp = avg2 * 10.0f;
    band2Upper = D2_ROUND(avg2 + bandTmp + 0.1f);
    band2Lower = D2_ROUND(avg2 - bandTmp);
    inBand2 = 0;                                         // -0x40（list2 带内）
    avg3 = 0.0f;
    avg3 = static_cast<float>(m_mapHitSum3) / static_cast<float>(m_mapHitCount);
    bandTmp = avg3 * 10.0f;
    band3Upper = D2_ROUND(avg3 + bandTmp + 0.1f);
    band3Lower = D2_ROUND(avg3 - bandTmp);
    inBand3 = 0;                                         // -0x30（list3 带内）
    inBand1 = 0;                                         // -0x2c（list1 带内）
    zero2 = 0;                                           // -0x28（list2 为 0）
    zero3 = 0;                                           // -0x24（list3 为 0）

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
}