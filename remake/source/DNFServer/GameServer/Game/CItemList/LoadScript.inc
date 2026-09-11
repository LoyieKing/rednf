// ── LoadScript（ORIG T 0x0832c65e）：RDAR 脚本节解析。
// [insert rarity] 丢 3 token；[rarity dicision] 写 3×4 网格；
// [level dicision]/[silver level dicision] 分别写 m_lvBonus[0]/[1]。
// [b28d-20] LoadScript（ORIG 0x0832c65e，222/218，nd 47）：残差 = 缺 ORIG
//   0x0832c6xx 的 `lea -0x4c; call C1Ev`（string 具名局部先声明后赋值形）与
//   其后的 bool 物化段（ORIG `movb $0; … call; xor $1; test; jne`），
//   4.4 把该 string 直构在 -0x50 且短路链折成正极性直跳 ⇒ P225/P28 族封顶。
// [b28d-solo-CItemList] AE（222/222，ndiff 0）| 本轮修：① scratch/token 声明序
//   交换（复现 ORIG token=-0x50/scratch=-0x4c）；② 补 dead bool `bScan`（复现
//   ORIG `movb $0,-0x35` 预清，P171/P198①）；③ while(cond)→for(;;)+
//   `if(!ScanType) break`（复现头条件 + `xor $1` 物化，P283/P168a）；
//   ④ CItemList.h `m_rarityDicision` int→unsigned int（P211 字段侧旋钮，
//   复现 RHS-first store；sizeof/偏移不变）。
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
}