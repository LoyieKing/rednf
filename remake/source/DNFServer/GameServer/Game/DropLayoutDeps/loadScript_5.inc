int CClearReward::loadScript(char const* path)
{
    // [SweepDropLayout 2026-09-09 复核] N82/N126 封顶（ORIG 0x085392c0，651/651；
    // §6.2 超大件三件套：p167 CLEAR 61=61、gap=0、槽位集差 8）。本轮试形记录：
    // ①[drop prob count] 的 fill 清零移入 for 循环内（贴 ORIG 0x853941f 的
    //   local_54/50/4c 每轮清零）——ndiff/槽集合不变，保留；
    // ②[drop prob] 内层独立 `std::string token`（贴 Ghidra C 的 local_60 do-while
    //   每轮构造/析构）——我方多 11 条（4.4 在 break/return 各点各发 D1，ORIG 单
    //   析构点 + iVar7 状态机），our 662 vs ORIG 651 反劣，已回退。
    // 残差=局部槽序（4.4 首槽常数 8：section/token -0x55/-0x54 vs -0x4d/-0x4c，
    // N82/N126）+ 多分支出口岛数
    //（P63/N108）。语义等价。
    // [L1FreeTop12.S2 2026-09-11] §6.5 复核：ORIG 21 串（含 ""/"[drop prob count]"/
    //   "[drop prob]"/"default".."pcroom bonus"/"[gold card cost table]"/"[gold card create
    //   rate]"/"[pcroom card blank item]"/"[reward item rate per map max count]"/"[gold card
    //   blank item]"）与源码逐条一致；错误码立即数集 ORIG−OURS=∅（0x943…0xa41 全等）；
    //   §6.5.1 enc 0 命中。
    if (!loadRDARScriptFile("", path))
        return 0x943;
    std::string section;
    std::string token;
    // [b28d-C08] ORIG 语句序 = section 构造(0x085392ef) → idx 清零(0x085392fa) →
    // ok 清零(0x08539301)：`idx` 声明**先于** `ok`。
    // [b28d-C08 clr_l/widthscan 判据⑫] ORIG 0x085392fa 在函数入口清 `-0x44(%ebp)`，
    // 而 [item drop ref table] 0x08539893 / [gold card cost table] 0x0853994f 都是
    // `mov %eax,-0x44(%ebp)`（ScanInt 结果复用同一槽）⇒ 原码是**函数头**具名
    // `int idx = 0;`，两段各自赋值（非循环内声明）。旧形两处 `int idx = ScanInt(&ok);`
    // 使 clr_l 少 1（8 vs 11）并整体抬高槽序。
    int idx = 0;
    bool ok = false;
    for (;;)
    {
        if (!ScanType(section, true))
            break;
        if (section == "[drop prob count]")
        {
            int count = ScanInt(&ok);
            if (!ok)
                return 0x951;
            for (int i = 0; i <= 5; ++i)
            {
                // [b28d-C08 试形记录] fill 提到函数头反致 +22 条（4.4 每轮额外拷贝），
                // 已回退；块内声明维持（槽 -0x58/-0x54/-0x50 vs ORIG -0x50/-0x4c/-0x48
                // 属 4.4 块内 POD 深槽策略，N82/N126 族）。
                // ORIG 0x08539373/7a/81：循环内三次**独立** `movl $0x0,-0x50/-0x4c/-0x48(%ebp)`。
                // 链式 `v[0]=v[1]=v[2]=0` 被 4.4 copy-prop 成 1 store + 2 copy（clr_l 8 vs 11）；
                // 三条独立语句才还原三次清零（widthscan clr_l 判据⑫）。
                stGenItemProb_t fill;
                fill.v[0] = 0;
                fill.v[1] = 0;
                fill.v[2] = 0;
                m_scriptProbs[i].resize(count, fill);
            }
        }
        else if (section == "[drop prob]")
        {
            // ScanStr 选槽：default/event/event2/pcroom default/pcroom event/
            // pcroom bonus → 0..5；未知 → 0x969。扫描失败静默退出本节。
            for (;;)
            {
                if (!ScanStr(&token))
                    break;
                int slot;
                if (token == "default")
                    slot = 0;
                else if (token == "event")
                    slot = 1;
                else if (token == "event2")
                    slot = 2;
                else if (token == "pcroom default")
                    slot = 3;
                else if (token == "pcroom event")
                    slot = 4;
                else if (token == "pcroom bonus")
                    slot = 5;
                else
                    return 0x969;
                for (size_t j = 0; j < m_scriptProbs[slot].size(); ++j)
                {
                    int* e = &m_scriptProbs[slot][j].v[0];
                    e[0] = ScanInt(&ok);
                    if (!ok)
                        return 0x96f;
                    e[1] = ScanInt(&ok);
                    if (!ok)
                        return 0x971;
                    e[2] = ScanInt(&ok);
                    if (!ok)
                        return 0x973;
                }
            }
        }
        else if (section == "[drop kind prob]")
        {
            m_dropKindProb[0] = ScanInt(&ok);
            if (!ok)
                return 0x97a;
            m_dropKindProb[1] = ScanInt(&ok);
            if (!ok)
                return 0x97c;
        }
        else if (section == "[drop item type prob]")
        {
            for (int i = 1; i <= 4; ++i)
            {
                m_dropItemTypeProb[i - 1] = ScanInt(&ok);   // ORIG 索引 1..4 → 下标 i-1
                if (!ok)
                    return 0x984;
            }
        }
        else if (section == "[basis of rarity dicision]")
        {
            for (int j = 0; j <= 5; ++j)
            {
                m_rarityBasis[j] = ScanInt(&ok);
                if (!ok)
                    return 0x98d;
            }
        }
        else if (section == "[dungeon difficulty drop bonusrate]")
        {
            for (int b = 0; b <= 4; ++b)
            {
                m_dungeonDropBonus[b] = ScanInt(&ok);
                if (!ok)
                    return 0x99a;
            }
        }
        else if (section == "[party member drop bonusrate]")
        {
            for (int f = 0; f <= 3; ++f)
            {
                m_partyBonus[f] = ScanFloat(&ok);
                if (!ok)
                    return 0x9a3;
            }
        }
        else if (section == "[dungeon difficulty gold drop bonusrate]")
        {
            for (int g = 0; g <= 4; ++g)
            {
                m_dungeonGoldBonus[g] = ScanFloat(&ok);
                if (!ok)
                    return 0x9b0;
            }
        }
        else if (section == "[item drop ref table]")
        {
            m_range.m_range[0] = 0;
            for (int lvl = 1; lvl <= 200; ++lvl)
            {
                idx = ScanInt(&ok);
                if (!ok)
                    return 0x9bc;
                unsigned char* entry =
                    reinterpret_cast<unsigned char*>(&m_range.m_range[idx]);
                entry[0] = (unsigned char)ScanInt(&ok);
                if (!ok)
                    return 0x9c0;
                entry[1] = (unsigned char)ScanInt(&ok);
                if (!ok)
                    return 0x9c4;
            }
        }
        else if (section == "[gold card cost table]")
        {
            // 扫描失败静默结束本节（ORIG 跳回节分发）。
            for (;;)
            {
                idx = ScanInt(&ok);
                if (!ok)
                    break;
                m_goldCardCost[idx] = ScanInt(&ok);
                if (!ok)
                    return 0x9d1;
            }
        }
        else if (section == "[gold card create rate]")
        {
            m_goldCardCreateRate = ScanFloat(&ok);
            if (!ok)
                return 0x9d8;
        }
        else if (section == "[pcroom card blank item]")
        {
            m_pcRoomBlankTotal = 0;
            for (;;)
            {
                stGoldCardBlankItemInfo_t info;
                info.v[0] = ScanInt(&ok);
                if (!ok)
                    break;
                info.v[1] = ScanInt(&ok);
                if (!ok)
                    return 0x9e6;
                info.v[2] = ScanInt(&ok);
                if (!ok)
                    return 0x9e9;
                m_pcRoomBlanks.push_back(info);
                m_pcRoomBlankTotal += info.v[0];   // ORIG：count += 首值
            }
        }
        else if (section == "[reward item rate per map max count]")
        {
            for (;;)
            {
                // [b28d-C08 轴 B / P196 正向] ORIG 0x08539ace-0x08539ad2 =
                // `cmpl $0x9,-0x10(%ebp); **jbe**`（**无符号**序）⇒ 该局部为 unsigned；
                // 我方 `int idx` 产有符号 `jle` ⇒ `idx<0` 时 ORIG 走 return 0x9f6 拒绝，
                // 我方却继续 `m_rewardRatePerMap[idx]`（负下标）⇒ **真语义分叉**，非纯 codegen。
                // 轴 B 来源：/tmp/r5/axisB_points.tsv 第 71 行（b_U->S，ORIG jbe vs OUR jle）。
                unsigned int idx = ScanInt(&ok);
                if (!ok)
                    break;
                if (idx > 9)
                    return 0x9f6;
                m_rewardRatePerMap[idx] = ScanInt(&ok);
                if (!ok)
                    return 0x9f9;
            }
        }
        else if (section == "[gold card blank item]")
        {
            m_goldCardBlankTotal = 0;
            for (;;)
            {
                stGoldCardBlankItemInfo_t info;
                info.v[0] = ScanInt(&ok);
                if (!ok)
                    break;
                info.v[1] = ScanInt(&ok);
                if (!ok)
                    return 0xa08;
                info.v[2] = ScanInt(&ok);
                if (!ok)
                    return 0xa0b;
                m_goldBlanks.push_back(info);
                m_goldCardBlankTotal += info.v[0];
            }
        }
        else
        {
            return 0xa41;
        }
    }
    m_table.generateTable(&m_range, false);
    return 0;
}