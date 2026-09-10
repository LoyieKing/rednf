int CCompound_ExtreamDun_Item::LoadScript(const char* dir, const char* path)
{
    if (!loadRDARScriptFile(dir, path))
    {
        return 0x62;
    }

    // [b28d-solo-CItemList] 槽序：ORIG token=-0x50/scratch=-0x4c（构造序
    //   scratch 先）⇒ 源码声明序 scratch, token（4.4 先声明浅）。
    std::string scratch;
    std::string token;
    // [b28d-solo-CItemList] P171/P198①：ORIG 0x832c65e 循环前有
    //   `movb $0x0,-0x35(%ebp)`（只写不读的 bool 局部预清）⇒ 补 dead bool
    //   声明复现该 store。
    bool bScan = false;
    (void)bScan;

    // [b28d-solo-CItemList] P283/P168a：ORIG 循环条件在**头**（`jmp L_cond;
    //   L_cond: call ScanType; xor $1; test; jne <exit>`），4.4 对 while(cond)
    //   恒旋转成尾条件 ⇒ 改 for(;;)+if(!ScanType) break 复现头条件与 xor 物化。
    for (;;)
    {
        if (!ScanType(token, true))
        {
            break;
        }
        if (token == "[insert rarity]")
        {
            for (int i = 0; i < 3; ++i)
            {
                ScanStr(&scratch);
            }
        }
        else if (token == "[rarity dicision]")
        {
            for (int i = 0; i < 3; ++i)
            {
                for (int j = 0; j < 4; ++j)
                {
                    m_data.m_rarityDicision[i][j] = ScanInt((bool*)0);
                }
            }
        }
        else if (token == "[level dicision]")
        {
            const unsigned int count = (unsigned int)ScanInt((bool*)0);
            for (unsigned int k = 0; k < count; ++k)
            {
                m_lvBonus[0].m_levels.push_back((unsigned int)ScanInt((bool*)0));
            }
            for (int r = 0; r < 3; ++r)
            {
                for (unsigned int k = 0; k < count; ++k)
                {
                    m_lvBonus[0].m_rates[r].push_back(
                        (unsigned int)ScanInt((bool*)0));
                }
            }
        }
        else if (token == "[silver level dicision]")
        {
            const unsigned int count = (unsigned int)ScanInt((bool*)0);
            for (unsigned int k = 0; k < count; ++k)
            {
                m_lvBonus[1].m_levels.push_back((unsigned int)ScanInt((bool*)0));
            }
            for (int r = 0; r < 3; ++r)
            {
                for (unsigned int k = 0; k < count; ++k)
                {
                    m_lvBonus[1].m_rates[r].push_back(
                        (unsigned int)ScanInt((bool*)0));
                }
            }
        }
    }
    return 0;
