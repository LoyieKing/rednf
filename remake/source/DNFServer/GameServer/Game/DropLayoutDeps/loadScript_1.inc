int CSpecialMonsterDrop::loadScript(char const* path)
{
    // [SweepDropLayout 2026-09-09 复核] N82附/N76 封顶（ORIG 0x085373ee，377/381，
    // ndiff 434→430）。具名 fill 改 resize 实参位匿名临时后槽差 0x18→4
    // （OURS ok -0x61/token -0x60 vs ORIG -0x5d/-0x5c；残 4 = N126/N95 首槽常数）。
    // p167 CLEAR（28=28）。已修：for(;;)+break、idx 循环前预清、全头部声明（P212）。
    // 逐段对照报告 Ghidra C 语义一致（含 e[1] 的 RHS-first 属 N76）。残差=槽常数 4；
    // ②`e[1] = ScanInt(&ok)` store 序 ORIG RHS-first `mov -S,%edx; mov %eax,0x4(%edx)`
    // vs 4.4 LHS-first `lea 0x4(%eax),%ebx` 跨 call 缓存（N76）；已试 P211 异型化
    // （e→unsigned int* + cast）反劣 +1 条，回退。语义等价。
    // [L1FreeTop12.S2 2026-09-11] §6.5 复核：ORIG 8 串（""/"[drop prob count]"/"[drop prob]"/
    //   "[basis of rarity dicision]"/"[party member drop bonusrate]"/"[dungeon difficulty drop
    //   bonusrate]"/"[monster type drop bonusrate]"/"[item drop ref table]"）与源码逐条一致；
    //   错误码立即数集 ORIG−OURS=∅（0x5fb…0x661 全等）；§6.5.1 enc 0 命中。
    if (!loadRDARScriptFile("", path))
        return 0x5fb;
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
    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[drop prob count]")
        {
            m_probs.resize(ScanInt(&ok), stGenItemProb_t());   // 试形：匿名临时
            if (!ok)
                return 0x609;
        }
        else if (token == "[drop prob]")
        {
            for (jd = 0; jd < m_probs.size(); ++jd)
            {
                e = &m_probs[jd].v[0];
                e[0] = ScanInt(&ok);
                if (!ok)
                    return 0x611;
                e[1] = ScanInt(&ok);
                if (!ok)
                    return 0x613;
                for (k = 0; k <= 4; ++k)
                {
                    e[2 + k] = ScanInt(&ok);
                    if (!ok)
                        return 0x617;
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
                        return 0x623;
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
                        return 0x62e;
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
                        return 0x63d;
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
                        return 0x648;
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
                    return 0x655;
                unsigned char* entry =
                    reinterpret_cast<unsigned char*>(&m_range.m_range[idx]);
                entry[0] = (unsigned char)ScanInt(&ok);
                if (!ok)
                    return 0x659;
                entry[1] = (unsigned char)ScanInt(&ok);
                if (!ok)
                    return 0x65d;
            }
        }
        else
        {
            return 0x661;
        }
    }
    m_table.generateTable(&m_range);
    return 0;
}