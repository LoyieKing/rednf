int CMonsterDrop_Hell::loadScript(char const* path)
{
    // [SweepDropLayout 2026-09-09 复核] N82附/N126 封顶（ORIG 0x08535344，277/280，
    // ndiff 299→295）。本轮把具名 `stGenItemProb_t fill`（4.4 恒占最深 -0x50..-0x35）
    // 改为 resize 实参位**匿名临时** `m_probs.resize(ScanInt(&ok), stGenItemProb_t())`
    // ⇒ 槽差由 0x18 缩到 4（OURS ok -0x4d/token -0x4c vs ORIG -0x49/-0x48；idx -0x2c
    // vs -0x28），残 4 = 4.4 首槽基址常数（N126/N95，P190 只能下移不能上移）。
    // 具名 fill 块内声明/头声明两形实测同槽，故取匿名形。逐段对照报告 Ghidra C 语义一致。
    // p167 CLEAR（23=23）。已修：for(;;)+break（顶部条件头）、i<rows+1（ORIG
    // add$1/setg 形）、idx 循环前预清（P198①）、全头部声明（P212）。残差=槽常数 4
    // + m_range[0] movw vs 两条 movb（P202/N78 族）+ N125（4.1 `add %eax,%eax` 两次
    // vs 4.4 shl）+ 多分支出口岛数（N108）。语义等价。
    // [L1FreeTop12.S2 2026-09-11] §6.5 复核：ORIG 5 串（""/"[basis of rarity dicision]"/
    //   "[drop prob count]"/"[dungeon difficulty drop prob]"/"[item drop ref table]"）与源码
    //   逐条一致；错误码立即数集 ORIG−OURS=∅（0x302…0x34b 全等）；§6.5.1 enc 0 命中。
    if (!loadRDARScriptFile("", path))
        return 0x302;
    std::string token;
    bool ok = false;
    int idx = 0;                             // ORIG 循环前预清（-0x28，P198①）
    int rows;
    int i;
    int j;
    size_t jd;
    int* e;
    int k;
    int lvl;
    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[basis of rarity dicision]")
        {
            rows = ScanInt(&ok);
            if (!ok)
                return 0x311;
            for (i = 1; i < rows + 1; ++i)   // ORIG add$1/setg 形
            {
                for (j = 0; j <= 5; ++j)
                {
                    m_rarityBasis[i][j] = ScanInt(&ok);
                    if (!ok)
                        return 0x319;
                }
            }
        }
        else if (token == "[drop prob count]")
        {
            m_probs.resize(ScanInt(&ok), stGenItemProb_t());   // 试形：匿名临时
            if (!ok)
                return 0x320;
        }
        else if (token == "[dungeon difficulty drop prob]")
        {
            for (jd = 0; jd < m_probs.size(); ++jd)
            {
                e = &m_probs[jd].v[0];
                e[0] = ScanInt(&ok);
                if (!ok)
                    return 0x328;
                e[1] = ScanInt(&ok);
                if (!ok)
                    return 0x32a;
                for (k = 0; k <= 4; ++k)
                {
                    e[2 + k] = ScanInt(&ok);
                    if (!ok)
                        return 0x332;
                }
            }
        }
        else if (token == "[item drop ref table]")
        {
            // ORIG 0x0853560e/0x08535615 为两条 movb；4.4 恒折单条 movw
            // （P202/N78 族）。字节视图形实测反劣 +5 条（地址 add 链），已回退。
            m_range.m_range[0] = 0;
            for (lvl = 1; lvl <= 200; ++lvl)
            {
                idx = ScanInt(&ok);
                if (!ok)
                    return 0x33f;
                unsigned char* entry =
                    reinterpret_cast<unsigned char*>(&m_range.m_range[idx]);
                entry[0] = (unsigned char)ScanInt(&ok);
                if (!ok)
                    return 0x343;
                entry[1] = (unsigned char)ScanInt(&ok);
                if (!ok)
                    return 0x347;
            }
        }
        else
        {
            return 0x34b;
        }
    }
    m_table.generateTable(&m_range, true);
    return 0;
}