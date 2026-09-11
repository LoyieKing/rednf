int CMonsterDrop::loadScript(char const* path)
{
    // [W13AStackableItem 2026-09-11 槽轴实证·具名变量区整体深 4，不可翻]
    // slotmap（影子 routed 实测）逐槽对照：ORIG 仅 -0x60(15)/-0x61(43) 两槽
    // ↔ 我方 -0x64(15)/-0x65(43)（token/ok），其余 -0x18..-0x58 全槽计数**逐条相同**
    // ⇒ 非「变量集不同」，而是我方具名局部区整体深 4 字节（4.4 于浅端多留 4）。
    // 本波 overlay 未再试形（前人已记录 resize 实参匿名化把槽差 0x18→4，见下）。
    // ⚠ 口径提醒（对全员判槽有用）：Ghidra 报告的 `local_65`/`local_64` 与 objdump 的
    //   `-0x61`/`-0x60` **差 4 是基准差**（Ghidra 以函数入口 esp 为基、objdump 以 ebp 为基），
    //   **不是**布局差异 —— 用 Ghidra local_NN 判我方槽位前必须减去 4。
    // [SweepDropLayout 2026-09-09 复核] N82附/N76 封顶（ORIG 0x08537938，540/545，
    // ndiff 549→545）。具名 fill 改 resize 实参位匿名临时后槽差 0x18→4
    // （OURS ok -0x65/token -0x64 vs ORIG -0x61/-0x60；残 4 = N126/N95 首槽常数）。
    // p167 CLEAR（42=42）。已修：for(;;)+break、count 内联实参、
    // [gold quantity] 展开为 ORIG 的 4 次独立 ScanInt
    // （原循环少 3 个调用点，p167 MISSING -3）、全头部声明（P212）。残差=槽常数 4
    // （N126/N95）+ `e[1] = ScanInt(&ok)` store 序 ORIG RHS-first vs 4.4 LHS-first
    // （N76，与 CSpecialMonsterDrop 同型）+ 多分支出口 jmp/nop 岛数（P63/N108）。
    //   逐段对照报告 Ghidra C 语义一致。语义等价。
    // [L1FreeTop12.S2 2026-09-11] §6.5 复核：ORIG 12 串（""/"[drop prob count]"/"[drop prob]"/
    //   "[basis of rarity dicision]"/"[party member drop bonusrate]"/"[dungeon difficulty drop
    //   bonusrate]"/"[monster type drop bonusrate]"/"[item drop ref table]"/"[first boss/named
    //   mob hunting]"/"[condition rate]"/"[gold quantity]"/"[gold volume]"）与源码逐条一致；
    //   错误码立即数集 ORIG−OURS=∅（0x66b…0x70a 全等）；§6.5.1 enc 0 命中。
    if (!loadRDARScriptFile("", path))
        return 0x66b;
    std::string token;
    bool ok = false;
    int idx = 0;
    size_t jd;
    int* e;
    int k;
    int i;
    int j;
    int i2;
    int j2;
    int b;
    int j3;
    int t;
    int j4;
    int lvl;
    int k2;
    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[drop prob count]")
        {
            m_probs.resize(ScanInt(&ok), stGenItemProb_t());   // 试形：匿名临时
            if (!ok)
                return 0x679;
        }
        else if (token == "[drop prob]")
        {
            for (jd = 0; jd < m_probs.size(); ++jd)
            {
                e = &m_probs[jd].v[0];
                e[0] = ScanInt(&ok);
                if (!ok)
                    return 0x681;
                e[1] = ScanInt(&ok);
                if (!ok)
                    return 0x683;
                for (k = 0; k <= 4; ++k)
                {
                    e[2 + k] = ScanInt(&ok);
                    if (!ok)
                        return 0x687;
                }
            }
        }
        else if (token == "[basis of rarity dicision]")
        {
            for (i = 1; i <= 4; ++i)
            {
                for (j = 0; j <= 5; ++j)
                {
                    m_rarityBasis[i][j] = ScanInt(&ok);
                    if (!ok)
                        return 0x693;
                }
            }
        }
        else if (token == "[party member drop bonusrate]")
        {
            for (i2 = 0; i2 <= 4; ++i2)
            {
                for (j2 = 0; j2 <= 3; ++j2)
                {
                    m_partyBonus[i2][j2] = ScanFloat(&ok);
                    if (!ok)
                        return 0x69e;
                }
            }
        }
        else if (token == "[dungeon difficulty drop bonusrate]")
        {
            for (b = 0; b <= 4; ++b)
            {
                for (j3 = 0; j3 <= 4; ++j3)
                {
                    m_dungeonBonus[b][j3] = ScanFloat(&ok);
                    if (!ok)
                        return 0x6ad;
                }
            }
        }
        else if (token == "[monster type drop bonusrate]")
        {
            for (t = 0; t <= 4; ++t)
            {
                for (j4 = 0; j4 <= 3; ++j4)
                {
                    m_monsterTypeBonus[t][j4] = ScanFloat(&ok);
                    if (!ok)
                        return 0x6b8;
                }
            }
        }
        else if (token == "[item drop ref table]")
        {
            m_range.m_range[0] = 0;
            for (lvl = 1; lvl <= 200; ++lvl)
            {
                idx = ScanInt(&ok);
                if (!ok)
                    return 0x6c5;
                unsigned char* entry =
                    reinterpret_cast<unsigned char*>(&m_range.m_range[idx]);
                entry[0] = (unsigned char)ScanInt(&ok);
                if (!ok)
                    return 0x6c9;
                entry[1] = (unsigned char)ScanInt(&ok);
                if (!ok)
                    return 0x6cd;
            }
        }
        else if (token == "[first boss/named mob hunting]")
        {
            // ORIG：先 [+0x25937c]=1 拷到 [+0x259378]，再两次扫描覆写。
            m_firstMobHunt.m_field04 = 1;
            m_firstMobHunt.m_weight = m_firstMobHunt.m_field04;
            m_firstMobHunt.m_weight = ScanInt(&ok);
            if (!ok)
                return 0x6d7;
            m_firstMobHunt.m_field04 = ScanInt(&ok);
            if (!ok)
                return 0x6db;
        }
        else if (token == "[condition rate]")
        {
            for (k2 = 0; k2 <= 3; ++k2)
            {
                m_conditionRate[k2].rate = (unsigned short)ScanInt(&ok);
                if (!ok)
                    return 0x6e5;
                m_conditionRate[k2].value = (unsigned short)ScanInt(&ok);
                if (!ok)
                    return 0x6e9;
            }
        }
        else if (token == "[gold quantity]")
        {
            // ORIG 展开 4 次 ScanInt（各带独立错误码，非循环）。
            m_goldQuantity[0] = (unsigned short)ScanInt(&ok);
            if (!ok)
                return 0x6f0;
            m_goldQuantity[1] = (unsigned short)ScanInt(&ok);
            if (!ok)
                return 0x6f4;
            m_goldQuantity[2] = (unsigned short)ScanInt(&ok);
            if (!ok)
                return 0x6f8;
            m_goldQuantity[3] = (unsigned short)ScanInt(&ok);
            if (!ok)
                return 0x6fc;
        }
        else if (token == "[gold volume]")
        {
            m_goldVolume[0] = (unsigned short)ScanInt(&ok);
            if (!ok)
                return 0x702;
            m_goldVolume[1] = (unsigned short)ScanInt(&ok);
            if (!ok)
                return 0x706;
        }
        else
        {
            return 0x70a;
        }
    }
    m_table.generateTable(&m_range, true);
    return 0;
}