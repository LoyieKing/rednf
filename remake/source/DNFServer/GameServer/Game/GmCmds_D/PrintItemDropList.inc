void CIndependentDrop::PrintItemDropList(
    const stMonsterIndependentDrop_t& drop, char* out, bool detail)
{
    // b28d-solo-GmCmds_D：ORIG 用内联 rep stos 清零（P214③/P253）⇒ `={0}` 形。
    char lineBuf[0x400] = {0};                               // -0x42c

    // ORIG `movzbl -0x43c,%eax; xor $1; test %al,%al; je <memcpy 块>`（P168a②）⇒
    // 源码为 `if (!detail) { find } else { memcpy }`。
    if (!detail)
    {
        CItem* item = G_CDataManager()->find_item(drop.m_itemIdx);
        if (item != 0)
        {
            sprintf(lineBuf, "%d\t%s\n", drop.m_itemIdx, item->GetItemName());
            strcat(out, lineBuf);
        }
    }
    else
    {
        memcpy(lineBuf, kListDrop, 0xd);
        strcat(out, lineBuf);
    }

    for (int i = 0; i < 5; ++i)
    {
        if (drop.m_rate[i] == 0)
        {
            continue;
        }
        sprintf(lineBuf, kDifficultyFmt, i);
        strcat(out, lineBuf);

        // ORIG 第一次 rate 无 float 槽（fildl/flds/fdivrp/fstpl 直落实参）⇒ 内联
        // 表达式；第二次 `fstps -0x14` 具名 float 局部供 4 次乘法（P252）。
        sprintf(lineBuf, kDropRateFmt,
                static_cast<double>(
                    static_cast<float>(drop.m_rate[i]) / 10000.0f));
        strcat(out, lineBuf);

        sprintf(lineBuf, kTryCountFmt, drop.m_rate[5], drop.m_rate[6],
                drop.m_rate[7], drop.m_rate[8]);
        strcat(out, lineBuf);

        const float rate =
            static_cast<float>(drop.m_rate[i]) / 10000.0f;   // ORIG fstps -0x14
        sprintf(lineBuf, kTotalDropRateFmt,
                static_cast<double>(static_cast<float>(drop.m_rate[5]) * rate),
                static_cast<double>(static_cast<float>(drop.m_rate[6]) * rate),
                static_cast<double>(static_cast<float>(drop.m_rate[7]) * rate),
                static_cast<double>(static_cast<float>(drop.m_rate[8]) * rate));
        strcat(out, lineBuf);

        sprintf(lineBuf, kTotalCountFmt, drop.m_rate[9]);
        strcat(out, lineBuf);

        if (detail)
        {
            memcpy(lineBuf, kItemList, 0xf);
            strcat(out, lineBuf);

            int prevAcc = 0;                                 // -0x10
            // ORIG 循环前一次性求值 begin(-0x24)/end(-0x20)（P13）。
            std::vector<std::pair<int, int> >::const_iterator pit =
                drop.m_pairList.begin();
            std::vector<std::pair<int, int> >::const_iterator pend =
                drop.m_pairList.end();
            for (; pit != pend; ++pit)
            {
                // [b28c-R26 复验] 已知取舍：ORIG 0x084b1a68 用 `*pit`（deEv，0x08235bba）
                // 且**不调** ptEv；写 `pit->first/pit->second` 会 p167 EXTRA ptEv ×2 /
                // MISSING deEv ×1（实测 calls 29 vs 30）。改用 `const std::pair<int,int>
                // p = *pit;` 可得 p167 CLEAR（29=29 实测），但**不再发射*** ORIG 实有
                // 的 `_ZNK9__gnu_cxx17__normal_iteratorIPKSt4pairIiiE...EptEv` 弱符号
                // 0x08193912 ⇒ TU identical 95→94（净负，实测）。故维持 `->` 形。
                const int itemId = pit->first;
                const int acc = pit->second;
                CItem* item = G_CDataManager()->find_item(itemId);
                if (item != 0)
                {
                    // ORIG segRate 无槽（fildl/flds/fdivrp 直落 sprintf 实参）。
                    sprintf(lineBuf, "%d\t%f\t%s\n", itemId,
                            static_cast<double>(
                                static_cast<float>(acc - prevAcc) / 10000.0f),
                            item->GetItemName());
                    strcat(out, lineBuf);
                    prevAcc = acc;
                }
            }
            prevAcc = 0;
        }
    }
